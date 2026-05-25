#include "bcf_orthogonal_camera.h"
using namespace godot;

void BCFOrthogonalCamera::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_view_point", "v"),          &BCFOrthogonalCamera::set_view_point);
    ClassDB::bind_method(D_METHOD("get_view_point"),               &BCFOrthogonalCamera::get_view_point);
    ClassDB::bind_method(D_METHOD("set_direction", "v"),           &BCFOrthogonalCamera::set_direction);
    ClassDB::bind_method(D_METHOD("get_direction"),                &BCFOrthogonalCamera::get_direction);
    ClassDB::bind_method(D_METHOD("set_up_vector", "v"),           &BCFOrthogonalCamera::set_up_vector);
    ClassDB::bind_method(D_METHOD("get_up_vector"),                &BCFOrthogonalCamera::get_up_vector);
    ClassDB::bind_method(D_METHOD("set_view_to_world_scale", "v"), &BCFOrthogonalCamera::set_view_to_world_scale);
    ClassDB::bind_method(D_METHOD("get_view_to_world_scale"),      &BCFOrthogonalCamera::get_view_to_world_scale);
    ClassDB::bind_method(D_METHOD("set_aspect_ratio", "v"),        &BCFOrthogonalCamera::set_aspect_ratio);
    ClassDB::bind_method(D_METHOD("get_aspect_ratio"),             &BCFOrthogonalCamera::get_aspect_ratio);

    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "view_point"),          "set_view_point",          "get_view_point");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "direction"),           "set_direction",           "get_direction");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "up_vector"),           "set_up_vector",           "get_up_vector");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT,   "view_to_world_scale"), "set_view_to_world_scale", "get_view_to_world_scale");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT,   "aspect_ratio"),        "set_aspect_ratio",        "get_aspect_ratio");
}
