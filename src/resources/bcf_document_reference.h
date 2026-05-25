#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFDocumentReference : public Resource {
    GDCLASS(BCFDocumentReference, Resource)
    String guid;
    String document_guid; // set when pointing to internal document
    String url;           // set when pointing to external URL
    String description;
protected:
    static void _bind_methods();
public:
    void set_guid(const String &v)          { guid = v; }
    String get_guid() const                 { return guid; }
    void set_document_guid(const String &v) { document_guid = v; }
    String get_document_guid() const        { return document_guid; }
    void set_url(const String &v)           { url = v; }
    String get_url() const                  { return url; }
    void set_description(const String &v)   { description = v; }
    String get_description() const          { return description; }
};
