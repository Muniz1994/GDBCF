#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFExtensions : public Resource {
    GDCLASS(BCFExtensions, Resource)

    Array topic_types;
    Array topic_statuses;
    Array priorities;
    Array labels;
    Array users;
    Array snippet_types;
    Array stages;

protected:
    static void _bind_methods();

public:
    void set_topic_types(const Array &v)    { topic_types = v; }
    Array get_topic_types() const           { return topic_types; }

    void set_topic_statuses(const Array &v) { topic_statuses = v; }
    Array get_topic_statuses() const        { return topic_statuses; }

    void set_priorities(const Array &v)     { priorities = v; }
    Array get_priorities() const            { return priorities; }

    void set_labels(const Array &v)         { labels = v; }
    Array get_labels() const                { return labels; }

    void set_users(const Array &v)          { users = v; }
    Array get_users() const                 { return users; }

    void set_snippet_types(const Array &v)  { snippet_types = v; }
    Array get_snippet_types() const         { return snippet_types; }

    void set_stages(const Array &v)         { stages = v; }
    Array get_stages() const                { return stages; }
};
