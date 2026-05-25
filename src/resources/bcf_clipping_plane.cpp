#include "bcf_clipping_plane.h"
using namespace godot;
void BCFClippingPlane::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_location", "v"),  &BCFClippingPlane::set_location);
    ClassDB::bind_method(D_METHOD("get_location"),       &BCFClippingPlane::get_location);
    ClassDB::bind_method(D_METHOD("set_direction", "v"), &BCFClippingPlane::set_direction);
    ClassDB::bind_method(D_METHOD("get_direction"),      &BCFClippingPlane::get_direction);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "location"),  "set_location",  "get_location");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "direction"), "set_direction", "get_direction");
}
