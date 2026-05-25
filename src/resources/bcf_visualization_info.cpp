#include "bcf_visualization_info.h"
using namespace godot;

void BCFVisualizationInfo::_bind_methods() {
    BIND_CONSTANT(BCF_CAMERA_NONE);
    BIND_CONSTANT(BCF_CAMERA_PERSPECTIVE);
    BIND_CONSTANT(BCF_CAMERA_ORTHOGONAL);

    ClassDB::bind_method(D_METHOD("set_guid", "v"),               &BCFVisualizationInfo::set_guid);
    ClassDB::bind_method(D_METHOD("get_guid"),                    &BCFVisualizationInfo::get_guid);
    ClassDB::bind_method(D_METHOD("set_components", "v"),         &BCFVisualizationInfo::set_components);
    ClassDB::bind_method(D_METHOD("get_components"),              &BCFVisualizationInfo::get_components);
    ClassDB::bind_method(D_METHOD("set_camera_type", "v"),        &BCFVisualizationInfo::set_camera_type);
    ClassDB::bind_method(D_METHOD("get_camera_type"),             &BCFVisualizationInfo::get_camera_type);
    ClassDB::bind_method(D_METHOD("set_perspective_camera", "v"), &BCFVisualizationInfo::set_perspective_camera);
    ClassDB::bind_method(D_METHOD("get_perspective_camera"),      &BCFVisualizationInfo::get_perspective_camera);
    ClassDB::bind_method(D_METHOD("set_orthogonal_camera", "v"),  &BCFVisualizationInfo::set_orthogonal_camera);
    ClassDB::bind_method(D_METHOD("get_orthogonal_camera"),       &BCFVisualizationInfo::get_orthogonal_camera);
    ClassDB::bind_method(D_METHOD("set_lines", "v"),              &BCFVisualizationInfo::set_lines);
    ClassDB::bind_method(D_METHOD("get_lines"),                   &BCFVisualizationInfo::get_lines);
    ClassDB::bind_method(D_METHOD("set_clipping_planes", "v"),    &BCFVisualizationInfo::set_clipping_planes);
    ClassDB::bind_method(D_METHOD("get_clipping_planes"),         &BCFVisualizationInfo::get_clipping_planes);
    ClassDB::bind_method(D_METHOD("set_bitmaps", "v"),            &BCFVisualizationInfo::set_bitmaps);
    ClassDB::bind_method(D_METHOD("get_bitmaps"),                 &BCFVisualizationInfo::get_bitmaps);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "guid"),                                                                              "set_guid",               "get_guid");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "components",         PROPERTY_HINT_RESOURCE_TYPE, "BCFComponents"),                 "set_components",         "get_components");
    ADD_PROPERTY(PropertyInfo(Variant::INT,    "camera_type"),                                                                       "set_camera_type",        "get_camera_type");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "perspective_camera", PROPERTY_HINT_RESOURCE_TYPE, "BCFPerspectiveCamera"),          "set_perspective_camera", "get_perspective_camera");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "orthogonal_camera",  PROPERTY_HINT_RESOURCE_TYPE, "BCFOrthogonalCamera"),           "set_orthogonal_camera",  "get_orthogonal_camera");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY,  "lines"),                                                                             "set_lines",              "get_lines");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY,  "clipping_planes"),                                                                   "set_clipping_planes",    "get_clipping_planes");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY,  "bitmaps"),                                                                           "set_bitmaps",            "get_bitmaps");
}
