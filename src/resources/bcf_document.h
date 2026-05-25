#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// Represents a document stored inside a BCF file.
// guid: must match the filename in the BCF container (per documents.xsd).
// data: the raw bytes of the file, loaded on parse.
class BCFDocument : public Resource {
    GDCLASS(BCFDocument, Resource)
    String          guid;
    String          filename;
    String          description;
    PackedByteArray data;
protected:
    static void _bind_methods();
public:
    void set_guid(const String &v)          { guid = v; }
    String get_guid() const                 { return guid; }
    void set_filename(const String &v)      { filename = v; }
    String get_filename() const             { return filename; }
    void set_description(const String &v)   { description = v; }
    String get_description() const          { return description; }
    void set_data(const PackedByteArray &v) { data = v; }
    PackedByteArray get_data() const        { return data; }
};
