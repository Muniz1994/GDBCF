#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFComment : public Resource {
    GDCLASS(BCFComment, Resource)

    String guid;
    String date;
    String author;
    String comment;
    String viewpoint_guid;
    String modified_date;
    String modified_author;

protected:
    static void _bind_methods();

public:
    void set_guid(const String &v)           { guid = v; }
    String get_guid() const                  { return guid; }

    void set_date(const String &v)           { date = v; }
    String get_date() const                  { return date; }

    void set_author(const String &v)         { author = v; }
    String get_author() const                { return author; }

    void set_comment(const String &v)        { comment = v; }
    String get_comment() const               { return comment; }

    void set_viewpoint_guid(const String &v) { viewpoint_guid = v; }
    String get_viewpoint_guid() const        { return viewpoint_guid; }

    void set_modified_date(const String &v)  { modified_date = v; }
    String get_modified_date() const         { return modified_date; }

    void set_modified_author(const String &v){ modified_author = v; }
    String get_modified_author() const       { return modified_author; }
};
