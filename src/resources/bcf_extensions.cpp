#include "bcf_extensions.h"
using namespace godot;

void BCFExtensions::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_topic_types", "v"),    &BCFExtensions::set_topic_types);
    ClassDB::bind_method(D_METHOD("get_topic_types"),         &BCFExtensions::get_topic_types);
    ClassDB::bind_method(D_METHOD("set_topic_statuses", "v"), &BCFExtensions::set_topic_statuses);
    ClassDB::bind_method(D_METHOD("get_topic_statuses"),      &BCFExtensions::get_topic_statuses);
    ClassDB::bind_method(D_METHOD("set_priorities", "v"),     &BCFExtensions::set_priorities);
    ClassDB::bind_method(D_METHOD("get_priorities"),          &BCFExtensions::get_priorities);
    ClassDB::bind_method(D_METHOD("set_labels", "v"),         &BCFExtensions::set_labels);
    ClassDB::bind_method(D_METHOD("get_labels"),              &BCFExtensions::get_labels);
    ClassDB::bind_method(D_METHOD("set_users", "v"),          &BCFExtensions::set_users);
    ClassDB::bind_method(D_METHOD("get_users"),               &BCFExtensions::get_users);
    ClassDB::bind_method(D_METHOD("set_snippet_types", "v"),  &BCFExtensions::set_snippet_types);
    ClassDB::bind_method(D_METHOD("get_snippet_types"),       &BCFExtensions::get_snippet_types);
    ClassDB::bind_method(D_METHOD("set_stages", "v"),         &BCFExtensions::set_stages);
    ClassDB::bind_method(D_METHOD("get_stages"),              &BCFExtensions::get_stages);

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "topic_types"),    "set_topic_types",    "get_topic_types");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "topic_statuses"), "set_topic_statuses", "get_topic_statuses");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "priorities"),     "set_priorities",     "get_priorities");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "labels"),         "set_labels",         "get_labels");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "users"),          "set_users",          "get_users");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "snippet_types"),  "set_snippet_types",  "get_snippet_types");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "stages"),         "set_stages",         "get_stages");
}
