#include "bcf_parser.h"

#include "tinyxml2.h"

#include "../resources/bcf_project.h"
#include "../resources/bcf_project_info.h"
#include "../resources/bcf_extensions.h"
#include "../resources/bcf_topic.h"
#include "../resources/bcf_comment.h"
#include "../resources/bcf_viewpoint_ref.h"
#include "../resources/bcf_visualization_info.h"
#include "../resources/bcf_components.h"
#include "../resources/bcf_component.h"
#include "../resources/bcf_component_visibility.h"
#include "../resources/bcf_component_color.h"
#include "../resources/bcf_perspective_camera.h"
#include "../resources/bcf_orthogonal_camera.h"
#include "../resources/bcf_line.h"
#include "../resources/bcf_clipping_plane.h"
#include "../resources/bcf_bitmap.h"
#include "../resources/bcf_bim_snippet.h"
#include "../resources/bcf_document_reference.h"
#include "../resources/bcf_document.h"
#include "../resources/bcf_header_file.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <set>

using namespace godot;
using namespace tinyxml2;

// ── Tiny helpers ──────────────────────────────────────────────────────────────

static String txt(const XMLElement *el) {
    if (!el || !el->GetText()) return String();
    return String(el->GetText());
}

static String attr(const XMLElement *el, const char *name, const char *def = "") {
    if (!el) return String(def);
    const char *v = el->Attribute(name);
    return v ? String(v) : String(def);
}

static bool attr_bool(const XMLElement *el, const char *name, bool def = false) {
    if (!el) return def;
    const char *v = el->Attribute(name);
    if (!v) return def;
    return (std::string(v) == "true" || std::string(v) == "1");
}

static double child_double(const XMLElement *parent, const char *child_name, double def = 0.0) {
    if (!parent) return def;
    const XMLElement *e = parent->FirstChildElement(child_name);
    if (!e || !e->GetText()) return def;
    return std::stod(e->GetText());
}

static Vector3 parse_vec3(const XMLElement *parent) {
    if (!parent) return Vector3();
    return Vector3(
        (float)child_double(parent, "X"),
        (float)child_double(parent, "Y"),
        (float)child_double(parent, "Z"));
}

static Ref<BCFComponent> parse_component(const XMLElement *el) {
    Ref<BCFComponent> c;
    c.instantiate();
    c->set_ifc_guid(attr(el, "IfcGuid"));
    if (auto *e = el->FirstChildElement("OriginatingSystem")) c->set_originating_system(txt(e));
    if (auto *e = el->FirstChildElement("AuthoringToolId"))   c->set_authoring_tool_id(txt(e));
    return c;
}

// ── Top-level parse ───────────────────────────────────────────────────────────

Ref<BCFProject> BCFParser::parse(BCFZip &zip) {
    // --- version check ---
    std::string ver_xml = zip.read_text_entry("bcf.version");
    if (!ver_xml.empty()) {
        XMLDocument doc;
        if (doc.Parse(ver_xml.c_str()) == XML_SUCCESS) {
            auto *root = doc.FirstChildElement("Version");
            if (root) {
                const char *vid = root->Attribute("VersionId");
                if (vid && std::string(vid) != "3.0") {
                    UtilityFunctions::push_warning(
                        String("GDBCF: BCF version ") + String(vid) +
                        String(" detected; only 3.0 is fully supported."));
                }
            }
        }
    }

    Ref<BCFProject> project = parse_project_info(zip);
    parse_extensions(zip, project);
    parse_documents(zip, project);

    // Enumerate topic folders: any entry of the form "{guid}/markup.bcf"
    std::set<std::string> topic_guids;
    for (const auto &entry : zip.list_entries()) {
        auto slash = entry.find('/');
        if (slash != std::string::npos) {
            std::string candidate = entry.substr(slash + 1);
            if (candidate == "markup.bcf") {
                topic_guids.insert(entry.substr(0, slash));
            }
        }
    }

    Array topics;
    for (const auto &guid : topic_guids) {
        auto topic = parse_topic(zip, guid);
        if (topic.is_valid()) topics.push_back(topic);
    }
    project->set_topics(topics);

    return project;
}

// ── Project info ──────────────────────────────────────────────────────────────

Ref<BCFProject> BCFParser::parse_project_info(BCFZip &zip) {
    Ref<BCFProject> project;
    project.instantiate();
    project->set_version("3.0");

    std::string xml = zip.read_text_entry("project.bcfp");
    if (xml.empty()) return project;

    XMLDocument doc;
    if (doc.Parse(xml.c_str()) != XML_SUCCESS) return project;

    auto *root = doc.FirstChildElement("ProjectInfo");
    if (!root) return project;
    auto *proj_el = root->FirstChildElement("Project");
    if (!proj_el) return project;

    Ref<BCFProjectInfo> info;
    info.instantiate();
    info->set_project_id(attr(proj_el, "ProjectId"));
    if (auto *name_el = proj_el->FirstChildElement("Name")) info->set_name(txt(name_el));
    project->set_project_info(info);

    return project;
}

// ── Extensions ────────────────────────────────────────────────────────────────

void BCFParser::parse_extensions(BCFZip &zip, Ref<BCFProject> project) {
    std::string xml = zip.read_text_entry("extensions.xml");
    if (xml.empty()) return;

    XMLDocument doc;
    if (doc.Parse(xml.c_str()) != XML_SUCCESS) return;

    auto *root = doc.FirstChildElement("Extensions");
    if (!root) return;

    Ref<BCFExtensions> ext;
    ext.instantiate();

    auto parse_list = [&](const char *container, const char *item) -> Array {
        Array arr;
        auto *c = root->FirstChildElement(container);
        if (!c) return arr;
        for (auto *e = c->FirstChildElement(item); e; e = e->NextSiblingElement(item))
            if (e->GetText()) arr.push_back(String(e->GetText()));
        return arr;
    };

    ext->set_topic_types(parse_list("TopicTypes", "TopicType"));
    ext->set_topic_statuses(parse_list("TopicStatuses", "TopicStatus"));
    ext->set_priorities(parse_list("Priorities", "Priority"));
    ext->set_labels(parse_list("TopicLabels", "TopicLabel"));
    ext->set_users(parse_list("Users", "User"));
    ext->set_snippet_types(parse_list("SnippetTypes", "SnippetType"));
    ext->set_stages(parse_list("Stages", "Stage"));

    project->set_extensions(ext);
}

// ── Documents ─────────────────────────────────────────────────────────────────

void BCFParser::parse_documents(BCFZip &zip, Ref<BCFProject> project) {
    std::string xml = zip.read_text_entry("documents.xml");
    if (xml.empty()) return;

    XMLDocument doc;
    if (doc.Parse(xml.c_str()) != XML_SUCCESS) return;

    auto *root = doc.FirstChildElement("DocumentInfo");
    if (!root) return;
    auto *docs_el = root->FirstChildElement("Documents");
    if (!docs_el) return;

    Array docs;
    for (auto *d = docs_el->FirstChildElement("Document"); d; d = d->NextSiblingElement("Document")) {
        Ref<BCFDocument> doc_res;
        doc_res.instantiate();
        doc_res->set_guid(attr(d, "Guid"));
        if (auto *fn = d->FirstChildElement("Filename"))    doc_res->set_filename(txt(fn));
        if (auto *de = d->FirstChildElement("Description")) doc_res->set_description(txt(de));

        // Try to load the file bytes from the ZIP (BCF v3 stores documents at {guid})
        auto bytes = zip.read_entry(doc_res->get_guid().utf8().get_data());
        if (!bytes.empty()) {
            PackedByteArray pba;
            pba.resize(static_cast<int64_t>(bytes.size()));
            memcpy(pba.ptrw(), bytes.data(), bytes.size());
            doc_res->set_data(pba);
        }
        docs.push_back(doc_res);
    }
    project->set_documents(docs);
}

// ── Topic ─────────────────────────────────────────────────────────────────────

Ref<BCFTopic> BCFParser::parse_topic(BCFZip &zip, const std::string &guid) {
    std::string xml = zip.read_text_entry(guid + "/markup.bcf");
    if (xml.empty()) return Ref<BCFTopic>();

    XMLDocument doc;
    if (doc.Parse(xml.c_str()) != XML_SUCCESS) return Ref<BCFTopic>();

    auto *markup_el = doc.FirstChildElement("Markup");
    if (!markup_el) return Ref<BCFTopic>();

    auto *topic_el = markup_el->FirstChildElement("Topic");
    if (!topic_el) return Ref<BCFTopic>();

    Ref<BCFTopic> topic;
    topic.instantiate();
    topic->set_guid(attr(topic_el, "Guid"));
    topic->set_server_assigned_id(attr(topic_el, "ServerAssignedId"));
    topic->set_topic_type(attr(topic_el, "TopicType"));
    topic->set_topic_status(attr(topic_el, "TopicStatus"));

    auto child_txt = [&](const char *name) -> String {
        auto *e = topic_el->FirstChildElement(name);
        return e ? txt(e) : String();
    };

    topic->set_title(child_txt("Title"));
    topic->set_description(child_txt("Description"));
    topic->set_creation_date(child_txt("CreationDate"));
    topic->set_creation_author(child_txt("CreationAuthor"));
    topic->set_modified_date(child_txt("ModifiedDate"));
    topic->set_modified_author(child_txt("ModifiedAuthor"));
    topic->set_due_date(child_txt("DueDate"));
    topic->set_assigned_to(child_txt("AssignedTo"));
    topic->set_priority(child_txt("Priority"));
    topic->set_stage(child_txt("Stage"));

    // Labels
    if (auto *labels_el = topic_el->FirstChildElement("Labels")) {
        Array labels;
        for (auto *l = labels_el->FirstChildElement("Label"); l; l = l->NextSiblingElement("Label"))
            if (l->GetText()) labels.push_back(String(l->GetText()));
        topic->set_labels(labels);
    }

    // Reference links
    if (auto *rl_el = topic_el->FirstChildElement("ReferenceLinks")) {
        Array links;
        for (auto *l = rl_el->FirstChildElement("ReferenceLink"); l; l = l->NextSiblingElement("ReferenceLink"))
            if (l->GetText()) links.push_back(String(l->GetText()));
        topic->set_reference_links(links);
    }

    // BimSnippet
    if (auto *bs = topic_el->FirstChildElement("BimSnippet")) {
        Ref<BCFBimSnippet> snippet;
        snippet.instantiate();
        snippet->set_snippet_type(attr(bs, "SnippetType"));
        snippet->set_is_external(attr_bool(bs, "IsExternal", false));
        if (auto *r = bs->FirstChildElement("Reference"))       snippet->set_reference(txt(r));
        if (auto *rs = bs->FirstChildElement("ReferenceSchema"))snippet->set_reference_schema(txt(rs));
        topic->set_bim_snippet(snippet);
    }

    // Document references
    if (auto *dr_el = topic_el->FirstChildElement("DocumentReferences")) {
        Array refs;
        for (auto *dr = dr_el->FirstChildElement("DocumentReference"); dr; dr = dr->NextSiblingElement("DocumentReference")) {
            Ref<BCFDocumentReference> r;
            r.instantiate();
            r->set_guid(attr(dr, "Guid"));
            if (auto *dg = dr->FirstChildElement("DocumentGuid")) r->set_document_guid(txt(dg));
            if (auto *url = dr->FirstChildElement("Url"))         r->set_url(txt(url));
            if (auto *de = dr->FirstChildElement("Description"))  r->set_description(txt(de));
            refs.push_back(r);
        }
        topic->set_document_references(refs);
    }

    // Related topics
    if (auto *rt_el = topic_el->FirstChildElement("RelatedTopics")) {
        Array related;
        for (auto *rt = rt_el->FirstChildElement("RelatedTopic"); rt; rt = rt->NextSiblingElement("RelatedTopic"))
            related.push_back(attr(rt, "Guid"));
        topic->set_related_topics(related);
    }

    // Header files
    auto *header_el = markup_el->FirstChildElement("Header");
    if (header_el) {
        auto *files_el = header_el->FirstChildElement("Files");
        if (files_el) {
            Array hfiles;
            for (auto *f = files_el->FirstChildElement("File"); f; f = f->NextSiblingElement("File")) {
                Ref<BCFHeaderFile> hf;
                hf.instantiate();
                hf->set_ifc_project(attr(f, "IfcProject"));
                hf->set_ifc_spatial_structure_element(attr(f, "IfcSpatialStructureElement"));
                hf->set_is_external(attr_bool(f, "IsExternal", true));
                if (auto *fn = f->FirstChildElement("Filename"))  hf->set_filename(txt(fn));
                if (auto *dt = f->FirstChildElement("Date"))      hf->set_date(txt(dt));
                if (auto *rf = f->FirstChildElement("Reference")) hf->set_reference(txt(rf));
                hfiles.push_back(hf);
            }
            topic->set_header_files(hfiles);
        }
    }

    // Comments
    if (auto *comments_el = markup_el->FirstChildElement("Comments")) {
        // Note: in v3.0 Comments is a child of Markup, not Topic
        Array comments;
        for (auto *c = comments_el->FirstChildElement("Comment"); c; c = c->NextSiblingElement("Comment")) {
            Ref<BCFComment> comment;
            comment.instantiate();
            comment->set_guid(attr(c, "Guid"));
            if (auto *d = c->FirstChildElement("Date"))           comment->set_date(txt(d));
            if (auto *a = c->FirstChildElement("Author"))         comment->set_author(txt(a));
            if (auto *t = c->FirstChildElement("Comment"))        comment->set_comment(txt(t));
            if (auto *vp = c->FirstChildElement("Viewpoint"))     comment->set_viewpoint_guid(attr(vp, "Guid"));
            if (auto *md = c->FirstChildElement("ModifiedDate"))  comment->set_modified_date(txt(md));
            if (auto *ma = c->FirstChildElement("ModifiedAuthor"))comment->set_modified_author(txt(ma));
            comments.push_back(comment);
        }
        topic->set_comments(comments);
    }

    // Also check Comments as child of Topic (some implementations differ)
    if (topic->get_comments().size() == 0) {
        if (auto *comments_el = topic_el->FirstChildElement("Comments")) {
            Array comments;
            for (auto *c = comments_el->FirstChildElement("Comment"); c; c = c->NextSiblingElement("Comment")) {
                Ref<BCFComment> comment;
                comment.instantiate();
                comment->set_guid(attr(c, "Guid"));
                if (auto *d = c->FirstChildElement("Date"))           comment->set_date(txt(d));
                if (auto *a = c->FirstChildElement("Author"))         comment->set_author(txt(a));
                if (auto *t = c->FirstChildElement("Comment"))        comment->set_comment(txt(t));
                if (auto *vp = c->FirstChildElement("Viewpoint"))     comment->set_viewpoint_guid(attr(vp, "Guid"));
                if (auto *md = c->FirstChildElement("ModifiedDate"))  comment->set_modified_date(txt(md));
                if (auto *ma = c->FirstChildElement("ModifiedAuthor"))comment->set_modified_author(txt(ma));
                comments.push_back(comment);
            }
            topic->set_comments(comments);
        }
    }

    // Viewpoints
    auto parse_vp_list = [&](const XMLElement *container_el) {
        if (!container_el) return;
        Array vps;
        for (auto *vp = container_el->FirstChildElement("ViewPoint");
             vp; vp = vp->NextSiblingElement("ViewPoint")) {
            Ref<BCFViewpointRef> vpref;
            vpref.instantiate();
            vpref->set_guid(attr(vp, "Guid"));
            if (auto *vf = vp->FirstChildElement("Viewpoint")) vpref->set_viewpoint_filename(txt(vf));
            if (auto *sf = vp->FirstChildElement("Snapshot"))  vpref->set_snapshot_filename(txt(sf));
            if (auto *idx = vp->FirstChildElement("Index")) {
                int i = 0; idx->QueryIntText(&i); vpref->set_index(i);
            }

            // Load .bcfv
            if (!vpref->get_viewpoint_filename().is_empty()) {
                std::string vp_entry = guid + "/" + vpref->get_viewpoint_filename().utf8().get_data();
                std::string vp_xml = zip.read_text_entry(vp_entry);
                if (!vp_xml.empty()) {
                    auto vis = parse_viewpoint(vp_xml);
                    if (vis.is_valid()) vpref->set_visualization_info(vis);
                }
            }

            // Load snapshot bytes
            if (!vpref->get_snapshot_filename().is_empty()) {
                std::string snap_entry = guid + "/" + vpref->get_snapshot_filename().utf8().get_data();
                auto snap_bytes = zip.read_entry(snap_entry);
                if (!snap_bytes.empty()) {
                    PackedByteArray pba;
                    pba.resize(static_cast<int64_t>(snap_bytes.size()));
                    memcpy(pba.ptrw(), snap_bytes.data(), snap_bytes.size());
                    vpref->set_snapshot_data(pba);
                }
            }
            vps.push_back(vpref);
        }
        topic->set_viewpoints(vps);
    };

    // Viewpoints may live under Markup or under Topic
    if (auto *vps_el = markup_el->FirstChildElement("Viewpoints")) {
        parse_vp_list(vps_el);
    } else if (auto *vps_el = topic_el->FirstChildElement("Viewpoints")) {
        parse_vp_list(vps_el);
    }

    return topic;
}

// ── Viewpoint (VisualizationInfo) ─────────────────────────────────────────────

Ref<BCFVisualizationInfo> BCFParser::parse_viewpoint(const std::string &xml_text) {
    Ref<BCFVisualizationInfo> vis;
    vis.instantiate();

    XMLDocument doc;
    if (doc.Parse(xml_text.c_str()) != XML_SUCCESS) return vis;

    auto *root = doc.FirstChildElement("VisualizationInfo");
    if (!root) return vis;

    vis->set_guid(attr(root, "Guid"));

    // Components
    if (auto *comp_el = root->FirstChildElement("Components")) {
        Ref<BCFComponents> comps;
        comps.instantiate();

        // Selection
        if (auto *sel_el = comp_el->FirstChildElement("Selection")) {
            Array sel;
            for (auto *c = sel_el->FirstChildElement("Component"); c; c = c->NextSiblingElement("Component"))
                sel.push_back(parse_component(c));
            comps->set_selection(sel);
        }

        // Visibility
        if (auto *vis_el = comp_el->FirstChildElement("Visibility")) {
            Ref<BCFComponentVisibility> cv;
            cv.instantiate();
            cv->set_default_visibility(attr_bool(vis_el, "DefaultVisibility", false));

            if (auto *hints = vis_el->FirstChildElement("ViewSetupHints")) {
                cv->set_spaces_visible(attr_bool(hints, "SpacesVisible", false));
                cv->set_space_boundaries_visible(attr_bool(hints, "SpaceBoundariesVisible", false));
                cv->set_openings_visible(attr_bool(hints, "OpeningsVisible", false));
            }

            if (auto *exc = vis_el->FirstChildElement("Exceptions")) {
                Array exceptions;
                for (auto *c = exc->FirstChildElement("Component"); c; c = c->NextSiblingElement("Component"))
                    exceptions.push_back(parse_component(c));
                cv->set_exceptions(exceptions);
            }
            comps->set_visibility(cv);
        }

        // Coloring
        if (auto *col_el = comp_el->FirstChildElement("Coloring")) {
            Array coloring;
            for (auto *c = col_el->FirstChildElement("Color"); c; c = c->NextSiblingElement("Color")) {
                Ref<BCFComponentColor> cc;
                cc.instantiate();
                cc->set_color(attr(c, "Color"));
                Array cc_comps;
                if (auto *comps_el = c->FirstChildElement("Components")) {
                    for (auto *comp = comps_el->FirstChildElement("Component"); comp; comp = comp->NextSiblingElement("Component"))
                        cc_comps.push_back(parse_component(comp));
                }
                cc->set_components(cc_comps);
                coloring.push_back(cc);
            }
            comps->set_coloring(coloring);
        }

        vis->set_components(comps);
    }

    // Camera (mutually exclusive: PerspectiveCamera or OrthogonalCamera)
    if (auto *pc = root->FirstChildElement("PerspectiveCamera")) {
        Ref<BCFPerspectiveCamera> cam;
        cam.instantiate();
        cam->set_view_point(parse_vec3(pc->FirstChildElement("CameraViewPoint")));
        cam->set_direction(parse_vec3(pc->FirstChildElement("CameraDirection")));
        cam->set_up_vector(parse_vec3(pc->FirstChildElement("CameraUpVector")));
        cam->set_fov(child_double(pc, "FieldOfView", 60.0));
        cam->set_aspect_ratio(child_double(pc, "AspectRatio", 1.0));
        vis->set_perspective_camera(cam);
        vis->set_camera_type(BCF_CAMERA_PERSPECTIVE);
    } else if (auto *oc = root->FirstChildElement("OrthogonalCamera")) {
        Ref<BCFOrthogonalCamera> cam;
        cam.instantiate();
        cam->set_view_point(parse_vec3(oc->FirstChildElement("CameraViewPoint")));
        cam->set_direction(parse_vec3(oc->FirstChildElement("CameraDirection")));
        cam->set_up_vector(parse_vec3(oc->FirstChildElement("CameraUpVector")));
        cam->set_view_to_world_scale(child_double(oc, "ViewToWorldScale", 1.0));
        cam->set_aspect_ratio(child_double(oc, "AspectRatio", 1.0));
        vis->set_orthogonal_camera(cam);
        vis->set_camera_type(BCF_CAMERA_ORTHOGONAL);
    }

    // Lines
    if (auto *lines_el = root->FirstChildElement("Lines")) {
        Array lines;
        for (auto *l = lines_el->FirstChildElement("Line"); l; l = l->NextSiblingElement("Line")) {
            Ref<BCFLine> line;
            line.instantiate();
            line->set_start_point(parse_vec3(l->FirstChildElement("StartPoint")));
            line->set_end_point(parse_vec3(l->FirstChildElement("EndPoint")));
            lines.push_back(line);
        }
        vis->set_lines(lines);
    }

    // Clipping planes
    if (auto *cp_el = root->FirstChildElement("ClippingPlanes")) {
        Array planes;
        for (auto *p = cp_el->FirstChildElement("ClippingPlane"); p; p = p->NextSiblingElement("ClippingPlane")) {
            Ref<BCFClippingPlane> plane;
            plane.instantiate();
            plane->set_location(parse_vec3(p->FirstChildElement("Location")));
            plane->set_direction(parse_vec3(p->FirstChildElement("Direction")));
            planes.push_back(plane);
        }
        vis->set_clipping_planes(planes);
    }

    // Bitmaps
    if (auto *bitmaps_el = root->FirstChildElement("Bitmaps")) {
        Array bitmaps;
        for (auto *b = bitmaps_el->FirstChildElement("Bitmap"); b; b = b->NextSiblingElement("Bitmap")) {
            Ref<BCFBitmap> bmp;
            bmp.instantiate();
            if (auto *fmt = b->FirstChildElement("Format"))    bmp->set_format(txt(fmt));
            if (auto *ref = b->FirstChildElement("Reference")) bmp->set_reference(txt(ref));
            bmp->set_location(parse_vec3(b->FirstChildElement("Location")));
            bmp->set_normal(parse_vec3(b->FirstChildElement("Normal")));
            bmp->set_up(parse_vec3(b->FirstChildElement("Up")));
            bmp->set_height(child_double(b, "Height", 1.0));
            bitmaps.push_back(bmp);
        }
        vis->set_bitmaps(bitmaps);
    }

    return vis;
}
