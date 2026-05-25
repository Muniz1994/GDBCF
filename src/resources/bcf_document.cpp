#include "bcf_document.h"
using namespace godot;
void BCFDocument::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_guid", "v"),        &BCFDocument::set_guid);
    ClassDB::bind_method(D_METHOD("get_guid"),             &BCFDocument::get_guid);
    ClassDB::bind_method(D_METHOD("set_filename", "v"),    &BCFDocument::set_filename);
    ClassDB::bind_method(D_METHOD("get_filename"),         &BCFDocument::get_filename);
    ClassDB::bind_method(D_METHOD("set_description", "v"), &BCFDocument::set_description);
    ClassDB::bind_method(D_METHOD("get_description"),      &BCFDocument::get_description);
    ClassDB::bind_method(D_METHOD("set_data", "v"),        &BCFDocument::set_data);
    ClassDB::bind_method(D_METHOD("get_data"),             &BCFDocument::get_data);

    ADD_PROPERTY(PropertyInfo(Variant::STRING,           "guid"),        "set_guid",        "get_guid");
    ADD_PROPERTY(PropertyInfo(Variant::STRING,           "filename"),    "set_filename",    "get_filename");
    ADD_PROPERTY(PropertyInfo(Variant::STRING,           "description"), "set_description", "get_description");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY,"data"),        "set_data",        "get_data");
}
