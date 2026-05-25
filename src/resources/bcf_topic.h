#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "bcf_bim_snippet.h"

using namespace godot;

class BCFComment;
class BCFViewpointRef;
class BCFDocumentReference;
class BCFHeaderFile;

class BCFTopic : public Resource {
    GDCLASS(BCFTopic, Resource)

    String guid;
    String server_assigned_id;
    String topic_type;
    String topic_status;
    String title;
    String description;
    String creation_date;
    String creation_author;
    String modified_date;
    String modified_author;
    String due_date;
    String assigned_to;
    String priority;
    Array  labels;           // Array[String]
    String stage;
    Array  reference_links;  // Array[String]
    Ref<BCFBimSnippet> bim_snippet;
    Array  document_references; // Array[BCFDocumentReference]
    Array  related_topics;      // Array[String] (GUIDs)
    Array  header_files;        // Array[BCFHeaderFile]
    Array  comments;            // Array[BCFComment]
    Array  viewpoints;          // Array[BCFViewpointRef]

protected:
    static void _bind_methods();

public:
    void set_guid(const String &v)                    { guid = v; }
    String get_guid() const                           { return guid; }

    void set_server_assigned_id(const String &v)      { server_assigned_id = v; }
    String get_server_assigned_id() const             { return server_assigned_id; }

    void set_topic_type(const String &v)              { topic_type = v; }
    String get_topic_type() const                     { return topic_type; }

    void set_topic_status(const String &v)            { topic_status = v; }
    String get_topic_status() const                   { return topic_status; }

    void set_title(const String &v)                   { title = v; }
    String get_title() const                          { return title; }

    void set_description(const String &v)             { description = v; }
    String get_description() const                    { return description; }

    void set_creation_date(const String &v)           { creation_date = v; }
    String get_creation_date() const                  { return creation_date; }

    void set_creation_author(const String &v)         { creation_author = v; }
    String get_creation_author() const                { return creation_author; }

    void set_modified_date(const String &v)           { modified_date = v; }
    String get_modified_date() const                  { return modified_date; }

    void set_modified_author(const String &v)         { modified_author = v; }
    String get_modified_author() const                { return modified_author; }

    void set_due_date(const String &v)                { due_date = v; }
    String get_due_date() const                       { return due_date; }

    void set_assigned_to(const String &v)             { assigned_to = v; }
    String get_assigned_to() const                    { return assigned_to; }

    void set_priority(const String &v)                { priority = v; }
    String get_priority() const                       { return priority; }

    void set_labels(const Array &v)                   { labels = v; }
    Array get_labels() const                          { return labels; }

    void set_stage(const String &v)                   { stage = v; }
    String get_stage() const                          { return stage; }

    void set_reference_links(const Array &v)          { reference_links = v; }
    Array get_reference_links() const                 { return reference_links; }

    void set_bim_snippet(const Ref<BCFBimSnippet> &v) { bim_snippet = v; }
    Ref<BCFBimSnippet> get_bim_snippet() const        { return bim_snippet; }

    void set_document_references(const Array &v)      { document_references = v; }
    Array get_document_references() const             { return document_references; }

    void set_related_topics(const Array &v)           { related_topics = v; }
    Array get_related_topics() const                  { return related_topics; }

    void set_header_files(const Array &v)             { header_files = v; }
    Array get_header_files() const                    { return header_files; }

    void set_comments(const Array &v)                 { comments = v; }
    Array get_comments() const                        { return comments; }

    void set_viewpoints(const Array &v)               { viewpoints = v; }
    Array get_viewpoints() const                      { return viewpoints; }
};
