#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFPerspectiveCamera : public Resource {
    GDCLASS(BCFPerspectiveCamera, Resource)

    Vector3 view_point;
    Vector3 direction;
    Vector3 up_vector;
    double  fov         = 60.0;
    double  aspect_ratio= 1.0;

protected:
    static void _bind_methods();

public:
    void set_view_point(const Vector3 &v)  { view_point = v; }
    Vector3 get_view_point() const         { return view_point; }

    void set_direction(const Vector3 &v)   { direction = v; }
    Vector3 get_direction() const          { return direction; }

    void set_up_vector(const Vector3 &v)   { up_vector = v; }
    Vector3 get_up_vector() const          { return up_vector; }

    void set_fov(double v)                 { fov = v; }
    double get_fov() const                 { return fov; }

    void set_aspect_ratio(double v)        { aspect_ratio = v; }
    double get_aspect_ratio() const        { return aspect_ratio; }
};
