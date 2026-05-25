#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// Forward declarations
class BCFProjectInfo;
class BCFExtensions;
class BCFTopic;
class BCFDocument;

class BCFProject : public Resource {
    GDCLASS(BCFProject, Resource)

    String version;
    Ref<BCFProjectInfo> project_info;
    Ref<BCFExtensions>  extensions;
    Array topics;    // Array[BCFTopic]
    Array documents; // Array[BCFDocument]

protected:
    static void _bind_methods();

public:
    void set_version(const String &p_v)                   { version = p_v; }
    String get_version() const                            { return version; }

    void set_project_info(const Ref<BCFProjectInfo> &p)   { project_info = p; }
    Ref<BCFProjectInfo> get_project_info() const          { return project_info; }

    void set_extensions(const Ref<BCFExtensions> &p)      { extensions = p; }
    Ref<BCFExtensions> get_extensions() const             { return extensions; }

    void set_topics(const Array &p)                       { topics = p; }
    Array get_topics() const                              { return topics; }

    void set_documents(const Array &p)                    { documents = p; }
    Array get_documents() const                           { return documents; }
};
