#include "bcf_components.h"
using namespace godot;

void BCFComponents::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_selection", "v"),   &BCFComponents::set_selection);
    ClassDB::bind_method(D_METHOD("get_selection"),        &BCFComponents::get_selection);
    ClassDB::bind_method(D_METHOD("set_visibility", "v"),  &BCFComponents::set_visibility);
    ClassDB::bind_method(D_METHOD("get_visibility"),       &BCFComponents::get_visibility);
    ClassDB::bind_method(D_METHOD("set_coloring", "v"),    &BCFComponents::set_coloring);
    ClassDB::bind_method(D_METHOD("get_coloring"),         &BCFComponents::get_coloring);

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY,  "selection"),                                                          "set_selection",  "get_selection");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "visibility", PROPERTY_HINT_RESOURCE_TYPE, "BCFComponentVisibility"), "set_visibility", "get_visibility");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY,  "coloring"),                                                           "set_coloring",   "get_coloring");
}
