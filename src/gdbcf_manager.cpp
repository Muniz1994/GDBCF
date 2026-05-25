#include "gdbcf_manager.h"

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
#include "io/bcf_zip.h"
#include "io/bcf_parser.h"
#include "io/bcf_writer.h"
#include "ifc_bridge/bcf_ifc_bridge.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/classes/time.hpp>

#include <random>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace godot;

// ── UUID v4 helper ────────────────────────────────────────────────────────────

static std::string make_uuid() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);

    uint32_t a = dist(gen);
    uint32_t b = dist(gen);
    uint32_t c = dist(gen);
    uint32_t d = dist(gen);

    // Set version 4 and variant bits
    b = (b & 0xFFFF0FFF) | 0x00004000; // version 4
    c = (c & 0x3FFFFFFF) | 0x80000000; // variant 1

    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(8) << a << '-'
        << std::setw(4) << ((b >> 16) & 0xFFFF) << '-'
        << std::setw(4) << (b & 0xFFFF) << '-'
        << std::setw(4) << ((c >> 16) & 0xFFFF) << '-'
        << std::setw(4) << (c & 0xFFFF)
        << std::setw(8) << d;
    return oss.str();
}

static std::string iso8601_now() {
    std::time_t t = std::time(nullptr);
    std::tm tm_utc{};
#if defined(_WIN32)
    gmtime_s(&tm_utc, &t);
#else
    gmtime_r(&t, &tm_utc);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm_utc);
    return buf;
}

// ── Constructor/Destructor ────────────────────────────────────────────────────

GDBCFManager::GDBCFManager() = default;

GDBCFManager::~GDBCFManager() {
    if (m_thread.joinable()) m_thread.join();
}

// ── Bind methods ──────────────────────────────────────────────────────────────

void GDBCFManager::_bind_methods() {
    // Async
    ClassDB::bind_method(D_METHOD("load_bcf", "path"),               &GDBCFManager::load_bcf);
    ClassDB::bind_method(D_METHOD("save_bcf", "project", "path"),    &GDBCFManager::save_bcf);
    // Sync
    ClassDB::bind_method(D_METHOD("load_bcf_sync", "path"),          &GDBCFManager::load_bcf_sync);
    ClassDB::bind_method(D_METHOD("save_bcf_sync", "project", "path"),&GDBCFManager::save_bcf_sync);
    // Factory
    ClassDB::bind_method(D_METHOD("create_project", "name"),         &GDBCFManager::create_project);
    ClassDB::bind_method(D_METHOD("create_topic", "type", "status", "title", "author"), &GDBCFManager::create_topic);
    ClassDB::bind_method(D_METHOD("generate_guid"),                  &GDBCFManager::generate_guid);
    // Bridge
    ClassDB::bind_method(D_METHOD("apply_viewpoint", "viewpoint_info", "ifc_root"), &GDBCFManager::apply_viewpoint);

    // Signals
    ADD_SIGNAL(MethodInfo("bcf_loaded",       PropertyInfo(Variant::OBJECT, "project", PROPERTY_HINT_RESOURCE_TYPE, "BCFProject")));
    ADD_SIGNAL(MethodInfo("bcf_saved"));
    ADD_SIGNAL(MethodInfo("error_occurred",   PropertyInfo(Variant::STRING, "message")));
}

// ── Async ─────────────────────────────────────────────────────────────────────

void GDBCFManager::load_bcf(const String &path) {
    if (m_busy.exchange(true)) {
        call_deferred("emit_signal", "error_occurred",
                      String("GDBCFManager: already busy with another operation."));
        return;
    }
    if (m_thread.joinable()) m_thread.join();

    m_thread = std::thread([this, path]() {
        auto project = load_bcf_sync(path);
        if (project.is_valid()) {
            call_deferred("emit_signal", "bcf_loaded", project);
        } else {
            call_deferred("emit_signal", "error_occurred",
                          String("Failed to load BCF file: ") + path);
        }
        m_busy.store(false);
    });
}

void GDBCFManager::save_bcf(const Ref<BCFProject> &project, const String &path) {
    if (m_busy.exchange(true)) {
        call_deferred("emit_signal", "error_occurred",
                      String("GDBCFManager: already busy with another operation."));
        return;
    }
    if (m_thread.joinable()) m_thread.join();

    m_thread = std::thread([this, project, path]() {
        int err = save_bcf_sync(project, path);
        if (err == 0) {
            call_deferred("emit_signal", "bcf_saved");
        } else {
            call_deferred("emit_signal", "error_occurred",
                          String("Failed to save BCF file: ") + path);
        }
        m_busy.store(false);
    });
}

// ── Sync ──────────────────────────────────────────────────────────────────────

Ref<BCFProject> GDBCFManager::load_bcf_sync(const String &path) {
    BCFZip zip;
    std::string path_std = path.utf8().get_data();
    if (!zip.open_read(path_std)) {
        UtilityFunctions::push_error(String("GDBCF: cannot open file: ") + path);
        return Ref<BCFProject>();
    }
    BCFParser parser;
    return parser.parse(zip);
}

int GDBCFManager::save_bcf_sync(const Ref<BCFProject> &project, const String &path) {
    if (!project.is_valid()) return 1;
    BCFZip zip;
    BCFWriter writer;
    if (!writer.write(project, zip)) return 1;
    if (!zip.write_to_file(path.utf8().get_data())) return 1;
    return 0;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Ref<BCFProject> GDBCFManager::create_project(const String &name) {
    Ref<BCFProject> project;
    project.instantiate();
    project->set_version("3.0");

    Ref<BCFProjectInfo> info;
    info.instantiate();
    info->set_project_id(String(make_uuid().c_str()));
    info->set_name(name);
    project->set_project_info(info);

    Ref<BCFExtensions> ext;
    ext.instantiate();
    Array topic_types, topic_statuses, priorities;
    topic_types.push_back(String("ERROR"));
    topic_types.push_back(String("WARNING"));
    topic_types.push_back(String("INFORMATION"));
    topic_types.push_back(String("CLASH"));
    topic_types.push_back(String("OTHER"));
    topic_statuses.push_back(String("OPEN"));
    topic_statuses.push_back(String("IN_PROGRESS"));
    topic_statuses.push_back(String("SOLVED"));
    topic_statuses.push_back(String("CLOSED"));
    priorities.push_back(String("LOW"));
    priorities.push_back(String("MEDIUM"));
    priorities.push_back(String("HIGH"));
    priorities.push_back(String("CRITICAL"));
    ext->set_topic_types(topic_types);
    ext->set_topic_statuses(topic_statuses);
    ext->set_priorities(priorities);
    project->set_extensions(ext);

    return project;
}

Ref<Resource> GDBCFManager::create_topic(
        const String &type, const String &status,
        const String &title, const String &author) {
    Ref<BCFTopic> topic;
    topic.instantiate();
    topic->set_guid(String(make_uuid().c_str()));
    topic->set_topic_type(type);
    topic->set_topic_status(status);
    topic->set_title(title);
    topic->set_creation_author(author);
    topic->set_creation_date(String(iso8601_now().c_str()));
    return topic;
}

String GDBCFManager::generate_guid() {
    return String(make_uuid().c_str());
}

// ── GDIFC bridge ──────────────────────────────────────────────────────────────

void GDBCFManager::apply_viewpoint(const Ref<Resource> &viewpoint_info, Node *ifc_root) {
    if (!viewpoint_info.is_valid() || !ifc_root) return;
    Ref<BCFVisualizationInfo> vis = viewpoint_info;
    if (!vis.is_valid()) return;
    BCFIfcBridge::apply_viewpoint(vis, ifc_root);
}
