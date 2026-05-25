#pragma once
#include <string>
#include "bcf_zip.h"
#include <godot_cpp/variant/utility_functions.hpp>

// Forward declarations
class BCFProject;
class BCFTopic;
class BCFVisualizationInfo;

namespace godot { template <class T> class Ref; }

// BCFParser: reads a BCFZip and constructs the Resource tree.
// All methods return null Refs on parse failure; partial results are still
// returned where possible so the caller can surface useful diagnostics.
class BCFParser {
public:
    // Top-level entry point.
    // Returns a fully populated BCFProject (or null on critical error).
    godot::Ref<BCFProject> parse(BCFZip &zip);

private:
    godot::Ref<BCFProject>          parse_project_info(BCFZip &zip);
    void                            parse_extensions(BCFZip &zip, godot::Ref<BCFProject> project);
    void                            parse_documents(BCFZip &zip, godot::Ref<BCFProject> project);
    godot::Ref<BCFTopic>            parse_topic(BCFZip &zip, const std::string &topic_guid);
    godot::Ref<BCFVisualizationInfo>parse_viewpoint(const std::string &xml_text);
};
