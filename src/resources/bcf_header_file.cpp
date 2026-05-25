#include "bcf_header_file.h"
using namespace godot;
void BCFHeaderFile::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_ifc_project", "v"),                   &BCFHeaderFile::set_ifc_project);
    ClassDB::bind_method(D_METHOD("get_ifc_project"),                        &BCFHeaderFile::get_ifc_project);
    ClassDB::bind_method(D_METHOD("set_ifc_spatial_structure_element", "v"), &BCFHeaderFile::set_ifc_spatial_structure_element);
    ClassDB::bind_method(D_METHOD("get_ifc_spatial_structure_element"),      &BCFHeaderFile::get_ifc_spatial_structure_element);
    ClassDB::bind_method(D_METHOD("set_is_external", "v"),                   &BCFHeaderFile::set_is_external);
    ClassDB::bind_method(D_METHOD("get_is_external"),                        &BCFHeaderFile::get_is_external);
    ClassDB::bind_method(D_METHOD("set_filename", "v"),                      &BCFHeaderFile::set_filename);
    ClassDB::bind_method(D_METHOD("get_filename"),                           &BCFHeaderFile::get_filename);
    ClassDB::bind_method(D_METHOD("set_date", "v"),                          &BCFHeaderFile::set_date);
    ClassDB::bind_method(D_METHOD("get_date"),                               &BCFHeaderFile::get_date);
    ClassDB::bind_method(D_METHOD("set_reference", "v"),                     &BCFHeaderFile::set_reference);
    ClassDB::bind_method(D_METHOD("get_reference"),                          &BCFHeaderFile::get_reference);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "ifc_project"),                   "set_ifc_project",                   "get_ifc_project");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "ifc_spatial_structure_element"), "set_ifc_spatial_structure_element", "get_ifc_spatial_structure_element");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL,   "is_external"),                   "set_is_external",                   "get_is_external");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "filename"),                       "set_filename",                      "get_filename");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "date"),                           "set_date",                          "get_date");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "reference"),                      "set_reference",                     "get_reference");
}
