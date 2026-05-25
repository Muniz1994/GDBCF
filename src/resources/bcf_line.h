#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFLine : public Resource {
    GDCLASS(BCFLine, Resource)
    Vector3 start_point;
    Vector3 end_point;
protected:
    static void _bind_methods();
public:
    void set_start_point(const Vector3 &v) { start_point = v; }
    Vector3 get_start_point() const        { return start_point; }
    void set_end_point(const Vector3 &v)   { end_point = v; }
    Vector3 get_end_point() const          { return end_point; }
};
