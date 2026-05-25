#include "register_types.h"

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

// Resources
#include "resources/bcf_project.h"
#include "resources/bcf_project_info.h"
#include "resources/bcf_extensions.h"
#include "resources/bcf_topic.h"
#include "resources/bcf_comment.h"
#include "resources/bcf_viewpoint_ref.h"
#include "resources/bcf_visualization_info.h"
#include "resources/bcf_components.h"
#include "resources/bcf_component.h"
#include "resources/bcf_component_visibility.h"
#include "resources/bcf_component_color.h"
#include "resources/bcf_perspective_camera.h"
#include "resources/bcf_orthogonal_camera.h"
#include "resources/bcf_line.h"
#include "resources/bcf_clipping_plane.h"
#include "resources/bcf_bitmap.h"
#include "resources/bcf_bim_snippet.h"
#include "resources/bcf_document_reference.h"
#include "resources/bcf_document.h"
#include "resources/bcf_header_file.h"

// Manager node
#include "gdbcf_manager.h"

using namespace godot;

void initialize_gdbcf_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    // Resources
    ClassDB::register_class<BCFProject>();
    ClassDB::register_class<BCFProjectInfo>();
    ClassDB::register_class<BCFExtensions>();
    ClassDB::register_class<BCFTopic>();
    ClassDB::register_class<BCFComment>();
    ClassDB::register_class<BCFViewpointRef>();
    ClassDB::register_class<BCFVisualizationInfo>();
    ClassDB::register_class<BCFComponents>();
    ClassDB::register_class<BCFComponent>();
    ClassDB::register_class<BCFComponentVisibility>();
    ClassDB::register_class<BCFComponentColor>();
    ClassDB::register_class<BCFPerspectiveCamera>();
    ClassDB::register_class<BCFOrthogonalCamera>();
    ClassDB::register_class<BCFLine>();
    ClassDB::register_class<BCFClippingPlane>();
    ClassDB::register_class<BCFBitmap>();
    ClassDB::register_class<BCFBimSnippet>();
    ClassDB::register_class<BCFDocumentReference>();
    ClassDB::register_class<BCFDocument>();
    ClassDB::register_class<BCFHeaderFile>();

    // Manager
    ClassDB::register_class<GDBCFManager>();
}

void uninitialize_gdbcf_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
GDExtensionBool GDE_EXPORT gdbcf_library_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization *r_initialization) {
    GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
    init_obj.register_initializer(initialize_gdbcf_module);
    init_obj.register_terminator(uninitialize_gdbcf_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
    return init_obj.init();
}
}
