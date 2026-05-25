#include "bcf_ifc_bridge.h"

#include "../resources/bcf_components.h"
#include "../resources/bcf_component.h"
#include "../resources/bcf_component_visibility.h"
#include "../resources/bcf_component_color.h"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <unordered_map>
#include <string>

using namespace godot;

// ── Build GUID map ────────────────────────────────────────────────────────────
// Walk the entire subtree; any node that exposes an "ifc_guid" String property
// is treated as an IFC node and indexed by its GUID value.

BCFIfcBridge::NodeMap BCFIfcBridge::build_guid_map(Node *root) {
    NodeMap map;
    if (!root) return map;

    // Iterative BFS to avoid deep recursion on large models.
    std::vector<Node *> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        Node *node = stack.back();
        stack.pop_back();

        // Check for "ifc_guid" property (duck-typed — works with GDIFC IFCNodes
        // and with any custom node that exposes the same property name).
        if (node->has_method("get_ifc_guid")) {
            Variant guid_val = node->call("get_ifc_guid");
            String guid = guid_val;
            if (!guid.is_empty()) {
                map[guid.utf8().get_data()] = node;
            }
        } else {
            // Fallback: check via property list (slower but universal).
            // godot-cpp 4.x: get_property_list() returns TypedArray<Dictionary>
            TypedArray<Dictionary> props = node->get_property_list();
            for (int pi = 0; pi < props.size(); pi++) {
                Dictionary d = props[pi];
                if (String(d["name"]) == "ifc_guid") {
                    Variant v = node->get("ifc_guid");
                    String guid = v;
                    if (!guid.is_empty()) {
                        map[guid.utf8().get_data()] = node;
                    }
                    break;
                }
            }
        }

        for (int i = 0; i < node->get_child_count(); i++) {
            stack.push_back(node->get_child(i));
        }
    }
    return map;
}

// ── apply_viewpoint ───────────────────────────────────────────────────────────

void BCFIfcBridge::apply_viewpoint(const Ref<BCFVisualizationInfo> &vis, Node *ifc_root) {
    if (!vis.is_valid() || !ifc_root) return;

    NodeMap map = build_guid_map(ifc_root);
    if (map.empty()) return;

    apply_visibility(vis, map, ifc_root);
    apply_coloring(vis, map);
}

// ── Visibility ────────────────────────────────────────────────────────────────

void BCFIfcBridge::apply_visibility(
        const Ref<BCFVisualizationInfo> &vis,
        const NodeMap &map,
        Node *root) {

    Ref<BCFComponents> comps = vis->get_components();
    if (!comps.is_valid()) return;

    Ref<BCFComponentVisibility> cv = comps->get_visibility();
    if (!cv.is_valid()) return;

    bool default_vis = cv->get_default_visibility();
    const Array &exceptions = cv->get_exceptions();

    // Build exception set
    std::unordered_map<std::string, bool> exception_map;
    for (int i = 0; i < exceptions.size(); i++) {
        Ref<BCFComponent> comp = exceptions[i];
        if (comp.is_valid() && !comp->get_ifc_guid().is_empty()) {
            exception_map[comp->get_ifc_guid().utf8().get_data()] = true;
        }
    }

    // Apply visibility to all mapped nodes
    for (const auto &[guid, node] : map) {
        bool is_exception = exception_map.count(guid) > 0;
        bool visible = is_exception ? !default_vis : default_vis;

        // Use Node3D::set_visible if available (MeshInstance3D, Node3D, etc.)
        if (auto *n3d = Object::cast_to<Node3D>(node)) {
            n3d->set_visible(visible);
        }
    }

    // Apply selection highlight (no dedicated overlay; use modulate)
    const Array &selection = comps->get_selection();
    for (int i = 0; i < selection.size(); i++) {
        Ref<BCFComponent> comp = selection[i];
        if (!comp.is_valid() || comp->get_ifc_guid().is_empty()) continue;
        auto it = map.find(comp->get_ifc_guid().utf8().get_data());
        if (it == map.end()) continue;

        Node *node = it->second;
        // Signal or call a highlight method if it exists (GDIFC pattern)
        if (node->has_method("set_highlight")) {
            node->call("set_highlight", true);
        } else if (auto *mi = Object::cast_to<MeshInstance3D>(node)) {
            // Fallback: slightly brighten via material override modulate
            // (non-destructive: we don't replace the material, just set modulate)
            mi->set("modulate", Color(1.5f, 1.5f, 0.5f, 1.0f));
        }
    }
}

// ── Coloring ──────────────────────────────────────────────────────────────────

void BCFIfcBridge::apply_coloring(
        const Ref<BCFVisualizationInfo> &vis,
        const NodeMap &map) {

    Ref<BCFComponents> comps = vis->get_components();
    if (!comps.is_valid()) return;

    const Array &coloring = comps->get_coloring();
    for (int i = 0; i < coloring.size(); i++) {
        Ref<BCFComponentColor> cc = coloring[i];
        if (!cc.is_valid()) continue;

        // Parse hex color: RRGGBB or AARRGGBB
        String hex = cc->get_color();
        Color color;
        if (hex.length() == 6) {
            color = Color::html("#" + hex);
        } else if (hex.length() == 8) {
            // BCF order is AARRGGBB
            int a = hex.substr(0, 2).hex_to_int();
            int r = hex.substr(2, 2).hex_to_int();
            int g = hex.substr(4, 2).hex_to_int();
            int b = hex.substr(6, 2).hex_to_int();
            color = Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        } else {
            continue;
        }

        const Array &cc_comps = cc->get_components();
        for (int j = 0; j < cc_comps.size(); j++) {
            Ref<BCFComponent> comp = cc_comps[j];
            if (!comp.is_valid() || comp->get_ifc_guid().is_empty()) continue;
            auto it = map.find(comp->get_ifc_guid().utf8().get_data());
            if (it == map.end()) continue;

            Node *node = it->second;
            if (node->has_method("set_highlight_color")) {
                node->call("set_highlight_color", color);
            } else if (auto *mi = Object::cast_to<MeshInstance3D>(node)) {
                mi->set("modulate", color);
            }
        }
    }
}
