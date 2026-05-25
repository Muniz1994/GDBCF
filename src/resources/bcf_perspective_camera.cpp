#include "bcf_perspective_camera.h"
using namespace godot;

void BCFPerspectiveCamera::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_view_point", "v"),   &BCFPerspectiveCamera::set_view_point);
    ClassDB::bind_method(D_METHOD("get_view_point"),        &BCFPerspectiveCamera::get_view_point);
    ClassDB::bind_method(D_METHOD("set_direction", "v"),    &BCFPerspectiveCamera::set_direction);
    ClassDB::bind_method(D_METHOD("get_direction"),         &BCFPerspectiveCamera::get_direction);
    ClassDB::bind_method(D_METHOD("set_up_vector", "v"),    &BCFPerspectiveCamera::set_up_vector);
    ClassDB::bind_method(D_METHOD("get_up_vector"),         &BCFPerspectiveCamera::get_up_vector);
    ClassDB::bind_method(D_METHOD("set_fov", "v"),          &BCFPerspectiveCamera::set_fov);
    ClassDB::bind_method(D_METHOD("get_fov"),               &BCFPerspectiveCamera::get_fov);
    ClassDB::bind_method(D_METHOD("set_aspect_ratio", "v"), &BCFPerspectiveCamera::set_aspect_ratio);
    ClassDB::bind_method(D_METHOD("get_aspect_ratio"),      &BCFPerspectiveCamera::get_aspect_ratio);

    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "view_point"),   "set_view_point",   "get_view_point");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "direction"),    "set_direction",    "get_direction");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "up_vector"),    "set_up_vector",    "get_up_vector");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT,   "fov"),          "set_fov",          "get_fov");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT,   "aspect_ratio"), "set_aspect_ratio", "get_aspect_ratio");
}
