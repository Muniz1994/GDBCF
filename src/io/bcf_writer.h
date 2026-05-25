#pragma once
#include "bcf_zip.h"

class BCFProject;
namespace godot { template <class T> class Ref; }

// BCFWriter: serializes a BCFProject Resource tree into a BCFZip.
class BCFWriter {
public:
    // Populate zip with all BCF entries. Call zip.write_to_file() afterwards.
    bool write(const godot::Ref<BCFProject> &project, BCFZip &zip);

private:
    void write_version(BCFZip &zip);
    void write_project_info(const godot::Ref<BCFProject> &project, BCFZip &zip);
    void write_extensions(const godot::Ref<BCFProject> &project, BCFZip &zip);
    void write_documents(const godot::Ref<BCFProject> &project, BCFZip &zip);
    void write_topic(const godot::Ref<class BCFTopic> &topic, BCFZip &zip);
    std::string write_viewpoint(const godot::Ref<class BCFVisualizationInfo> &vis);
};
