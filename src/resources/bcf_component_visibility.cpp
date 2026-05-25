#include "bcf_component_visibility.h"
using namespace godot;

void BCFComponentVisibility::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_default_visibility", "v"),       &BCFComponentVisibility::set_default_visibility);
    ClassDB::bind_method(D_METHOD("get_default_visibility"),            &BCFComponentVisibility::get_default_visibility);
    ClassDB::bind_method(D_METHOD("set_exceptions", "v"),               &BCFComponentVisibility::set_exceptions);
    ClassDB::bind_method(D_METHOD("get_exceptions"),                    &BCFComponentVisibility::get_exceptions);
    ClassDB::bind_method(D_METHOD("set_spaces_visible", "v"),           &BCFComponentVisibility::set_spaces_visible);
    ClassDB::bind_method(D_METHOD("get_spaces_visible"),                &BCFComponentVisibility::get_spaces_visible);
    ClassDB::bind_method(D_METHOD("set_space_boundaries_visible", "v"), &BCFComponentVisibility::set_space_boundaries_visible);
    ClassDB::bind_method(D_METHOD("get_space_boundaries_visible"),      &BCFComponentVisibility::get_space_boundaries_visible);
    ClassDB::bind_method(D_METHOD("set_openings_visible", "v"),         &BCFComponentVisibility::set_openings_visible);
    ClassDB::bind_method(D_METHOD("get_openings_visible"),              &BCFComponentVisibility::get_openings_visible);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "default_visibility"),      "set_default_visibility",      "get_default_visibility");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY,"exceptions"),              "set_exceptions",              "get_exceptions");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "spaces_visible"),          "set_spaces_visible",          "get_spaces_visible");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "space_boundaries_visible"),"set_space_boundaries_visible","get_space_boundaries_visible");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "openings_visible"),        "set_openings_visible",        "get_openings_visible");
}
