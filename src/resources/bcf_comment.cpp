#include "bcf_comment.h"
using namespace godot;

void BCFComment::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_guid", "v"),            &BCFComment::set_guid);
    ClassDB::bind_method(D_METHOD("get_guid"),                 &BCFComment::get_guid);
    ClassDB::bind_method(D_METHOD("set_date", "v"),            &BCFComment::set_date);
    ClassDB::bind_method(D_METHOD("get_date"),                 &BCFComment::get_date);
    ClassDB::bind_method(D_METHOD("set_author", "v"),          &BCFComment::set_author);
    ClassDB::bind_method(D_METHOD("get_author"),               &BCFComment::get_author);
    ClassDB::bind_method(D_METHOD("set_comment", "v"),         &BCFComment::set_comment);
    ClassDB::bind_method(D_METHOD("get_comment"),              &BCFComment::get_comment);
    ClassDB::bind_method(D_METHOD("set_viewpoint_guid", "v"),  &BCFComment::set_viewpoint_guid);
    ClassDB::bind_method(D_METHOD("get_viewpoint_guid"),       &BCFComment::get_viewpoint_guid);
    ClassDB::bind_method(D_METHOD("set_modified_date", "v"),   &BCFComment::set_modified_date);
    ClassDB::bind_method(D_METHOD("get_modified_date"),        &BCFComment::get_modified_date);
    ClassDB::bind_method(D_METHOD("set_modified_author", "v"), &BCFComment::set_modified_author);
    ClassDB::bind_method(D_METHOD("get_modified_author"),      &BCFComment::get_modified_author);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "guid"),           "set_guid",           "get_guid");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "date"),           "set_date",           "get_date");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "author"),         "set_author",         "get_author");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "comment"),        "set_comment",        "get_comment");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "viewpoint_guid"), "set_viewpoint_guid", "get_viewpoint_guid");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "modified_date"),  "set_modified_date",  "get_modified_date");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "modified_author"),"set_modified_author","get_modified_author");
}
