#include "bcf_project.h"
#include "bcf_project_info.h"
#include "bcf_extensions.h"

using namespace godot;

void BCFProject::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_version", "version"), &BCFProject::set_version);
    ClassDB::bind_method(D_METHOD("get_version"), &BCFProject::get_version);

    ClassDB::bind_method(D_METHOD("set_project_info", "info"), &BCFProject::set_project_info);
    ClassDB::bind_method(D_METHOD("get_project_info"), &BCFProject::get_project_info);

    ClassDB::bind_method(D_METHOD("set_extensions", "extensions"), &BCFProject::set_extensions);
    ClassDB::bind_method(D_METHOD("get_extensions"), &BCFProject::get_extensions);

    ClassDB::bind_method(D_METHOD("set_topics", "topics"), &BCFProject::set_topics);
    ClassDB::bind_method(D_METHOD("get_topics"), &BCFProject::get_topics);

    ClassDB::bind_method(D_METHOD("set_documents", "documents"), &BCFProject::set_documents);
    ClassDB::bind_method(D_METHOD("get_documents"), &BCFProject::get_documents);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "version"), "set_version", "get_version");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "project_info", PROPERTY_HINT_RESOURCE_TYPE, "BCFProjectInfo"), "set_project_info", "get_project_info");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "extensions", PROPERTY_HINT_RESOURCE_TYPE, "BCFExtensions"), "set_extensions", "get_extensions");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "topics"), "set_topics", "get_topics");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "documents"), "set_documents", "get_documents");
}
