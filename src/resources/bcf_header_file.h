#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFHeaderFile : public Resource {
    GDCLASS(BCFHeaderFile, Resource)
    String ifc_project;
    String ifc_spatial_structure_element;
    bool   is_external = true;
    String filename;
    String date;
    String m_reference;
protected:
    static void _bind_methods();
public:
    void set_ifc_project(const String &v)                    { ifc_project = v; }
    String get_ifc_project() const                           { return ifc_project; }
    void set_ifc_spatial_structure_element(const String &v)  { ifc_spatial_structure_element = v; }
    String get_ifc_spatial_structure_element() const         { return ifc_spatial_structure_element; }
    void set_is_external(bool v)                             { is_external = v; }
    bool get_is_external() const                             { return is_external; }
    void set_filename(const String &v)                       { filename = v; }
    String get_filename() const                              { return filename; }
    void set_date(const String &v)                           { date = v; }
    String get_date() const                                  { return date; }
    void set_reference(const String &v)                      { m_reference = v; }
    String get_reference() const                             { return m_reference; }
};
