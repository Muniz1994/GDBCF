#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

// camera_type enum
#define BCF_CAMERA_NONE        0
#define BCF_CAMERA_PERSPECTIVE 1
#define BCF_CAMERA_ORTHOGONAL  2

using namespace godot;

class BCFVisualizationInfo : public Resource {
    GDCLASS(BCFVisualizationInfo, Resource)

    String        guid;
    Ref<Resource> components;         // BCFComponents
    int           camera_type = BCF_CAMERA_NONE;
    Ref<Resource> perspective_camera; // BCFPerspectiveCamera
    Ref<Resource> orthogonal_camera;  // BCFOrthogonalCamera
    Array         lines;              // Array[BCFLine]
    Array         clipping_planes;    // Array[BCFClippingPlane]
    Array         bitmaps;            // Array[BCFBitmap]

protected:
    static void _bind_methods();

public:
    void set_guid(const String &v)                    { guid = v; }
    String get_guid() const                           { return guid; }

    void set_components(const Ref<Resource> &v)       { components = v; }
    Ref<Resource> get_components() const              { return components; }

    void set_camera_type(int v)                       { camera_type = v; }
    int get_camera_type() const                       { return camera_type; }

    void set_perspective_camera(const Ref<Resource> &v){ perspective_camera = v; }
    Ref<Resource> get_perspective_camera() const      { return perspective_camera; }

    void set_orthogonal_camera(const Ref<Resource> &v){ orthogonal_camera = v; }
    Ref<Resource> get_orthogonal_camera() const       { return orthogonal_camera; }

    void set_lines(const Array &v)                    { lines = v; }
    Array get_lines() const                           { return lines; }

    void set_clipping_planes(const Array &v)          { clipping_planes = v; }
    Array get_clipping_planes() const                 { return clipping_planes; }

    void set_bitmaps(const Array &v)                  { bitmaps = v; }
    Array get_bitmaps() const                         { return bitmaps; }
};
