#include "bcf_viewpoint_ref.h"
using namespace godot;

void BCFViewpointRef::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_guid", "v"),                  &BCFViewpointRef::set_guid);
    ClassDB::bind_method(D_METHOD("get_guid"),                       &BCFViewpointRef::get_guid);
    ClassDB::bind_method(D_METHOD("set_viewpoint_filename", "v"),    &BCFViewpointRef::set_viewpoint_filename);
    ClassDB::bind_method(D_METHOD("get_viewpoint_filename"),         &BCFViewpointRef::get_viewpoint_filename);
    ClassDB::bind_method(D_METHOD("set_snapshot_filename", "v"),     &BCFViewpointRef::set_snapshot_filename);
    ClassDB::bind_method(D_METHOD("get_snapshot_filename"),          &BCFViewpointRef::get_snapshot_filename);
    ClassDB::bind_method(D_METHOD("set_snapshot_data", "v"),         &BCFViewpointRef::set_snapshot_data);
    ClassDB::bind_method(D_METHOD("get_snapshot_data"),              &BCFViewpointRef::get_snapshot_data);
    ClassDB::bind_method(D_METHOD("set_index", "v"),                 &BCFViewpointRef::set_index);
    ClassDB::bind_method(D_METHOD("get_index"),                      &BCFViewpointRef::get_index);
    ClassDB::bind_method(D_METHOD("set_visualization_info", "v"),    &BCFViewpointRef::set_visualization_info);
    ClassDB::bind_method(D_METHOD("get_visualization_info"),         &BCFViewpointRef::get_visualization_info);

    ADD_PROPERTY(PropertyInfo(Variant::STRING,           "guid"),               "set_guid",               "get_guid");
    ADD_PROPERTY(PropertyInfo(Variant::STRING,           "viewpoint_filename"), "set_viewpoint_filename", "get_viewpoint_filename");
    ADD_PROPERTY(PropertyInfo(Variant::STRING,           "snapshot_filename"),  "set_snapshot_filename",  "get_snapshot_filename");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY,"snapshot_data"),      "set_snapshot_data",      "get_snapshot_data");
    ADD_PROPERTY(PropertyInfo(Variant::INT,              "index"),              "set_index",              "get_index");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT,           "visualization_info",  PROPERTY_HINT_RESOURCE_TYPE, "BCFVisualizationInfo"), "set_visualization_info", "get_visualization_info");
}
