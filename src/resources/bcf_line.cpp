#include "bcf_line.h"
using namespace godot;
void BCFLine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_start_point", "v"), &BCFLine::set_start_point);
    ClassDB::bind_method(D_METHOD("get_start_point"),      &BCFLine::get_start_point);
    ClassDB::bind_method(D_METHOD("set_end_point", "v"),   &BCFLine::set_end_point);
    ClassDB::bind_method(D_METHOD("get_end_point"),        &BCFLine::get_end_point);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "start_point"), "set_start_point", "get_start_point");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "end_point"),   "set_end_point",   "get_end_point");
}
