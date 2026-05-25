#include "bcf_component_color.h"
using namespace godot;

void BCFComponentColor::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_color", "v"),      &BCFComponentColor::set_color);
    ClassDB::bind_method(D_METHOD("get_color"),           &BCFComponentColor::get_color);
    ClassDB::bind_method(D_METHOD("set_components", "v"), &BCFComponentColor::set_components);
    ClassDB::bind_method(D_METHOD("get_components"),      &BCFComponentColor::get_components);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "color"),      "set_color",      "get_color");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY,  "components"), "set_components", "get_components");
}
