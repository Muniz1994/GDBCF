#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFClippingPlane : public Resource {
    GDCLASS(BCFClippingPlane, Resource)
    Vector3 location;
    Vector3 direction;
protected:
    static void _bind_methods();
public:
    void set_location(const Vector3 &v)  { location = v; }
    Vector3 get_location() const         { return location; }
    void set_direction(const Vector3 &v) { direction = v; }
    Vector3 get_direction() const        { return direction; }
};
