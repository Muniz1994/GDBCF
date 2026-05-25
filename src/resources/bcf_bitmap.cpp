#include "bcf_bitmap.h"
using namespace godot;
void BCFBitmap::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_format", "v"),    &BCFBitmap::set_format);
    ClassDB::bind_method(D_METHOD("get_format"),         &BCFBitmap::get_format);
    ClassDB::bind_method(D_METHOD("set_reference", "v"), &BCFBitmap::set_reference);
    ClassDB::bind_method(D_METHOD("get_reference"),      &BCFBitmap::get_reference);
    ClassDB::bind_method(D_METHOD("set_location", "v"),  &BCFBitmap::set_location);
    ClassDB::bind_method(D_METHOD("get_location"),       &BCFBitmap::get_location);
    ClassDB::bind_method(D_METHOD("set_normal", "v"),    &BCFBitmap::set_normal);
    ClassDB::bind_method(D_METHOD("get_normal"),         &BCFBitmap::get_normal);
    ClassDB::bind_method(D_METHOD("set_up", "v"),        &BCFBitmap::set_up);
    ClassDB::bind_method(D_METHOD("get_up"),             &BCFBitmap::get_up);
    ClassDB::bind_method(D_METHOD("set_height", "v"),    &BCFBitmap::set_height);
    ClassDB::bind_method(D_METHOD("get_height"),         &BCFBitmap::get_height);

    ADD_PROPERTY(PropertyInfo(Variant::STRING,  "format"),    "set_format",    "get_format");
    ADD_PROPERTY(PropertyInfo(Variant::STRING,  "reference"), "set_reference", "get_reference");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "location"),  "set_location",  "get_location");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "normal"),    "set_normal",    "get_normal");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "up"),        "set_up",        "get_up");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT,   "height"),    "set_height",    "get_height");
}
