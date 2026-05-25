#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFViewpointRef : public Resource {
    GDCLASS(BCFViewpointRef, Resource)

    String             guid;
    String             viewpoint_filename;
    String             snapshot_filename;
    PackedByteArray    snapshot_data;   // raw PNG/JPG bytes
    int                index = -1;
    Ref<Resource>      visualization_info; // BCFVisualizationInfo

protected:
    static void _bind_methods();

public:
    void set_guid(const String &v)                      { guid = v; }
    String get_guid() const                             { return guid; }

    void set_viewpoint_filename(const String &v)        { viewpoint_filename = v; }
    String get_viewpoint_filename() const               { return viewpoint_filename; }

    void set_snapshot_filename(const String &v)         { snapshot_filename = v; }
    String get_snapshot_filename() const                { return snapshot_filename; }

    void set_snapshot_data(const PackedByteArray &v)    { snapshot_data = v; }
    PackedByteArray get_snapshot_data() const           { return snapshot_data; }

    void set_index(int v)                               { index = v; }
    int get_index() const                               { return index; }

    void set_visualization_info(const Ref<Resource> &v) { visualization_info = v; }
    Ref<Resource> get_visualization_info() const        { return visualization_info; }
};
