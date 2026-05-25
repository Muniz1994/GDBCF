#pragma once
#include <godot_cpp/classes/node.hpp>
#include "../resources/bcf_visualization_info.h"

// BCFIfcBridge: duck-typed bridge between BCF viewpoints and GDIFC nodes.
//
// Works WITHOUT a hard compile-time dependency on the GDIFC extension headers.
// It discovers IFCNodes at runtime by checking for the "ifc_guid" property on
// every Node in the ifc_root subtree, so it compiles and ships standalone.
//
// When GDIFC IS loaded alongside GDBCF, the bridge will automatically find and
// manipulate IFCNode instances via Godot's property/method system.
class BCFIfcBridge {
public:
    // Walk ifc_root, apply selection / visibility / coloring from vis.
    static void apply_viewpoint(
        const godot::Ref<BCFVisualizationInfo> &vis,
        godot::Node *ifc_root);

private:
    using NodeMap = std::unordered_map<std::string, godot::Node *>;

    // Build map: ifc_guid → Node from the full subtree.
    static NodeMap build_guid_map(godot::Node *root);

    static void apply_visibility(
        const godot::Ref<BCFVisualizationInfo> &vis,
        const NodeMap &map,
        godot::Node *root);

    static void apply_coloring(
        const godot::Ref<BCFVisualizationInfo> &vis,
        const NodeMap &map);
};
