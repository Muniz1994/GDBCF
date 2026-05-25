#include "bcf_project_info.h"
using namespace godot;

void BCFProjectInfo::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_project_id", "id"), &BCFProjectInfo::set_project_id);
    ClassDB::bind_method(D_METHOD("get_project_id"), &BCFProjectInfo::get_project_id);

    ClassDB::bind_method(D_METHOD("set_name", "name"), &BCFProjectInfo::set_name);
    ClassDB::bind_method(D_METHOD("get_name"), &BCFProjectInfo::get_name);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "project_id"), "set_project_id", "get_project_id");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
}
