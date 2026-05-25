#pragma once
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <thread>
#include <atomic>

class BCFProject;

class GDBCFManager : public godot::Node {
    GDCLASS(GDBCFManager, godot::Node)

    std::thread       m_thread;
    std::atomic<bool> m_busy{false};

protected:
    static void _bind_methods();

public:
    GDBCFManager();
    ~GDBCFManager();

    // ── Async API ──────────────────────────────────────────────────────────
    // Emits bcf_loaded(project) or error_occurred(message) when done.
    void load_bcf(const godot::String &path);

    // Emits bcf_saved() or error_occurred(message) when done.
    void save_bcf(const godot::Ref<BCFProject> &project, const godot::String &path);

    // ── Sync API (blocking, suitable for editor tools) ────────────────────
    godot::Ref<BCFProject> load_bcf_sync(const godot::String &path);
    int save_bcf_sync(const godot::Ref<BCFProject> &project, const godot::String &path);

    // ── Factory helpers ───────────────────────────────────────────────────
    godot::Ref<BCFProject> create_project(const godot::String &name);
    godot::Ref<godot::Resource> create_topic(
        const godot::String &type,
        const godot::String &status,
        const godot::String &title,
        const godot::String &author);
    godot::String generate_guid();

    // ── GDIFC bridge ──────────────────────────────────────────────────────
    // Walk ifc_root, find nodes with "ifc_guid" property, apply viewpoint.
    void apply_viewpoint(const godot::Ref<godot::Resource> &viewpoint_info,
                         godot::Node *ifc_root);
};
