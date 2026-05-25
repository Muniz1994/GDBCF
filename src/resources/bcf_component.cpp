#include "bcf_component.h"
using namespace godot;

void BCFComponent::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_ifc_guid", "v"),          &BCFComponent::set_ifc_guid);
    ClassDB::bind_method(D_METHOD("get_ifc_guid"),               &BCFComponent::get_ifc_guid);
    ClassDB::bind_method(D_METHOD("set_originating_system", "v"),&BCFComponent::set_originating_system);
    ClassDB::bind_method(D_METHOD("get_originating_system"),     &BCFComponent::get_originating_system);
    ClassDB::bind_method(D_METHOD("set_authoring_tool_id", "v"), &BCFComponent::set_authoring_tool_id);
    ClassDB::bind_method(D_METHOD("get_authoring_tool_id"),      &BCFComponent::get_authoring_tool_id);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "ifc_guid"),          "set_ifc_guid",          "get_ifc_guid");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "originating_system"),"set_originating_system","get_originating_system");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "authoring_tool_id"), "set_authoring_tool_id", "get_authoring_tool_id");
}
