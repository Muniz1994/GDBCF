#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFBimSnippet : public Resource {
    GDCLASS(BCFBimSnippet, Resource)
    String snippet_type;
    bool   is_external = false;
    String m_reference;
    String reference_schema;
protected:
    static void _bind_methods();
public:
    void set_snippet_type(const String &v)    { snippet_type = v; }
    String get_snippet_type() const           { return snippet_type; }
    void set_is_external(bool v)              { is_external = v; }
    bool get_is_external() const              { return is_external; }
    void set_reference(const String &v)       { m_reference = v; }
    String get_reference() const              { return m_reference; }
    void set_reference_schema(const String &v){ reference_schema = v; }
    String get_reference_schema() const       { return reference_schema; }
};
