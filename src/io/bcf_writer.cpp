#include "bcf_writer.h"

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

#include <cstring>

using namespace godot;
using namespace tinyxml2;

// ── Tiny helpers ──────────────────────────────────────────────────────────────

static std::string to_std(const String &s) {
    return s.utf8().get_data();
}

static XMLElement *add_text(XMLDocument &doc, XMLElement *parent,
                            const char *tag, const std::string &text) {
    if (text.empty()) return nullptr;
    auto *el = doc.NewElement(tag);
    el->SetText(text.c_str());
    parent->InsertEndChild(el);
    return el;
}

static std::string doc_to_string(XMLDocument &doc) {
    XMLPrinter printer;
    doc.Print(&printer);
    return printer.CStr();
}

static void add_vec3(XMLDocument &doc, XMLElement *parent, const char *tag, Vector3 v) {
    auto *el = doc.NewElement(tag);
    auto *x = doc.NewElement("X"); x->SetText(v.x); el->InsertEndChild(x);
    auto *y = doc.NewElement("Y"); y->SetText(v.y); el->InsertEndChild(y);
    auto *z = doc.NewElement("Z"); z->SetText(v.z); el->InsertEndChild(z);
    parent->InsertEndChild(el);
}

static void write_component_el(XMLDocument &doc, XMLElement *parent, const Ref<BCFComponent> &comp) {
    auto *el = doc.NewElement("Component");
    if (!comp->get_ifc_guid().is_empty())
        el->SetAttribute("IfcGuid", to_std(comp->get_ifc_guid()).c_str());
    if (!comp->get_originating_system().is_empty())
        add_text(doc, el, "OriginatingSystem", to_std(comp->get_originating_system()));
    if (!comp->get_authoring_tool_id().is_empty())
        add_text(doc, el, "AuthoringToolId", to_std(comp->get_authoring_tool_id()));
    parent->InsertEndChild(el);
}

// ── Top-level ─────────────────────────────────────────────────────────────────

bool BCFWriter::write(const Ref<BCFProject> &project, BCFZip &zip) {
    if (!project.is_valid()) return false;

    zip.open_write();
    write_version(zip);
    write_project_info(project, zip);
    write_extensions(project, zip);
    write_documents(project, zip);

    const Array &topics = project->get_topics();
    for (int i = 0; i < topics.size(); i++) {
        Ref<BCFTopic> topic = topics[i];
        if (topic.is_valid()) write_topic(topic, zip);
    }
    return true;
}

// ── bcf.version ───────────────────────────────────────────────────────────────

void BCFWriter::write_version(BCFZip &zip) {
    XMLDocument doc;
    doc.InsertEndChild(doc.NewDeclaration());
    auto *root = doc.NewElement("Version");
    root->SetAttribute("VersionId", "3.0");
    doc.InsertEndChild(root);
    zip.add_text_entry("bcf.version", doc_to_string(doc));
}

// ── project.bcfp ─────────────────────────────────────────────────────────────

void BCFWriter::write_project_info(const Ref<BCFProject> &project, BCFZip &zip) {
    Ref<BCFProjectInfo> info = project->get_project_info();
    if (!info.is_valid()) return;
    if (info->get_project_id().is_empty() && info->get_name().is_empty()) return;

    XMLDocument doc;
    doc.InsertEndChild(doc.NewDeclaration());
    auto *root = doc.NewElement("ProjectInfo");
    auto *proj = doc.NewElement("Project");
    proj->SetAttribute("ProjectId", to_std(info->get_project_id()).c_str());
    add_text(doc, proj, "Name", to_std(info->get_name()));
    root->InsertEndChild(proj);
    doc.InsertEndChild(root);
    zip.add_text_entry("project.bcfp", doc_to_string(doc));
}

// ── extensions.xml ────────────────────────────────────────────────────────────

void BCFWriter::write_extensions(const Ref<BCFProject> &project, BCFZip &zip) {
    Ref<BCFExtensions> ext = project->get_extensions();
    if (!ext.is_valid()) return;

    XMLDocument doc;
    doc.InsertEndChild(doc.NewDeclaration());
    auto *root = doc.NewElement("Extensions");

    auto write_list = [&](const char *container, const char *item, const Array &arr) {
        if (arr.size() == 0) return;
        auto *c = doc.NewElement(container);
        for (int i = 0; i < arr.size(); i++) {
            auto *el = doc.NewElement(item);
            el->SetText(to_std(String(arr[i])).c_str());
            c->InsertEndChild(el);
        }
        root->InsertEndChild(c);
    };

    write_list("TopicTypes",    "TopicType",   ext->get_topic_types());
    write_list("TopicStatuses", "TopicStatus", ext->get_topic_statuses());
    write_list("Priorities",    "Priority",    ext->get_priorities());
    write_list("TopicLabels",   "TopicLabel",  ext->get_labels());
    write_list("Users",         "User",        ext->get_users());
    write_list("SnippetTypes",  "SnippetType", ext->get_snippet_types());
    write_list("Stages",        "Stage",       ext->get_stages());

    doc.InsertEndChild(root);
    zip.add_text_entry("extensions.xml", doc_to_string(doc));
}

// ── documents.xml ─────────────────────────────────────────────────────────────

void BCFWriter::write_documents(const Ref<BCFProject> &project, BCFZip &zip) {
    const Array &docs = project->get_documents();
    if (docs.size() == 0) return;

    XMLDocument doc;
    doc.InsertEndChild(doc.NewDeclaration());
    auto *root = doc.NewElement("DocumentInfo");
    auto *docs_el = doc.NewElement("Documents");

    for (int i = 0; i < docs.size(); i++) {
        Ref<BCFDocument> d = docs[i];
        if (!d.is_valid()) continue;
        auto *del = doc.NewElement("Document");
        del->SetAttribute("Guid", to_std(d->get_guid()).c_str());
        add_text(doc, del, "Filename",    to_std(d->get_filename()));
        add_text(doc, del, "Description", to_std(d->get_description()));
        docs_el->InsertEndChild(del);

        // Store document data in the ZIP if present
        const PackedByteArray &data = d->get_data();
        if (data.size() > 0) {
            zip.add_entry(to_std(d->get_guid()),
                          std::vector<uint8_t>(data.ptr(), data.ptr() + data.size()));
        }
    }
    root->InsertEndChild(docs_el);
    doc.InsertEndChild(root);
    zip.add_text_entry("documents.xml", doc_to_string(doc));
}

// ── {guid}/markup.bcf ────────────────────────────────────────────────────────

void BCFWriter::write_topic(const Ref<BCFTopic> &topic, BCFZip &zip) {
    std::string guid = to_std(topic->get_guid());
    std::string folder = guid + "/";

    XMLDocument doc;
    doc.InsertEndChild(doc.NewDeclaration());
    auto *markup_el = doc.NewElement("Markup");

    // Header
    const Array &hfiles = topic->get_header_files();
    if (hfiles.size() > 0) {
        auto *header_el = doc.NewElement("Header");
        auto *files_el  = doc.NewElement("Files");
        for (int i = 0; i < hfiles.size(); i++) {
            Ref<BCFHeaderFile> hf = hfiles[i];
            if (!hf.is_valid()) continue;
            auto *f = doc.NewElement("File");
            if (!hf->get_ifc_project().is_empty())
                f->SetAttribute("IfcProject", to_std(hf->get_ifc_project()).c_str());
            if (!hf->get_ifc_spatial_structure_element().is_empty())
                f->SetAttribute("IfcSpatialStructureElement",
                                to_std(hf->get_ifc_spatial_structure_element()).c_str());
            f->SetAttribute("IsExternal", hf->get_is_external());
            add_text(doc, f, "Filename",  to_std(hf->get_filename()));
            add_text(doc, f, "Date",      to_std(hf->get_date()));
            add_text(doc, f, "Reference", to_std(hf->get_reference()));
            files_el->InsertEndChild(f);
        }
        header_el->InsertEndChild(files_el);
        markup_el->InsertEndChild(header_el);
    }

    // Topic element
    auto *topic_el = doc.NewElement("Topic");
    topic_el->SetAttribute("Guid",           to_std(topic->get_guid()).c_str());
    topic_el->SetAttribute("TopicType",      to_std(topic->get_topic_type()).c_str());
    topic_el->SetAttribute("TopicStatus",    to_std(topic->get_topic_status()).c_str());
    if (!topic->get_server_assigned_id().is_empty())
        topic_el->SetAttribute("ServerAssignedId", to_std(topic->get_server_assigned_id()).c_str());

    // Reference links
    const Array &rlinks = topic->get_reference_links();
    if (rlinks.size() > 0) {
        auto *rl_el = doc.NewElement("ReferenceLinks");
        for (int i = 0; i < rlinks.size(); i++)
            add_text(doc, rl_el, "ReferenceLink", to_std(String(rlinks[i])));
        topic_el->InsertEndChild(rl_el);
    }

    add_text(doc, topic_el, "Title",          to_std(topic->get_title()));
    add_text(doc, topic_el, "Priority",       to_std(topic->get_priority()));

    // Labels — must appear before CreationDate per markup.xsd xs:sequence
    const Array &labels = topic->get_labels();
    if (labels.size() > 0) {
        auto *labels_el = doc.NewElement("Labels");
        for (int i = 0; i < labels.size(); i++)
            add_text(doc, labels_el, "Label", to_std(String(labels[i])));
        topic_el->InsertEndChild(labels_el);
    }

    add_text(doc, topic_el, "CreationDate",   to_std(topic->get_creation_date()));
    // CreationAuthor is required (NonEmptyOrBlankString, minOccurs=1); guard against empty
    {
        std::string ca = to_std(topic->get_creation_author());
        if (ca.empty()) ca = "editor";
        add_text(doc, topic_el, "CreationAuthor", ca);
    }
    add_text(doc, topic_el, "ModifiedDate",   to_std(topic->get_modified_date()));
    add_text(doc, topic_el, "ModifiedAuthor", to_std(topic->get_modified_author()));
    add_text(doc, topic_el, "DueDate",        to_std(topic->get_due_date()));
    add_text(doc, topic_el, "AssignedTo",     to_std(topic->get_assigned_to()));
    add_text(doc, topic_el, "Stage",          to_std(topic->get_stage()));
    add_text(doc, topic_el, "Description",    to_std(topic->get_description()));

    // BimSnippet
    Ref<BCFBimSnippet> snippet = topic->get_bim_snippet();
    if (snippet.is_valid() && !snippet->get_snippet_type().is_empty()) {
        auto *bs = doc.NewElement("BimSnippet");
        bs->SetAttribute("SnippetType", to_std(snippet->get_snippet_type()).c_str());
        bs->SetAttribute("IsExternal",  snippet->get_is_external());
        add_text(doc, bs, "Reference",       to_std(snippet->get_reference()));
        add_text(doc, bs, "ReferenceSchema", to_std(snippet->get_reference_schema()));
        topic_el->InsertEndChild(bs);
    }

    // Document references
    const Array &drefs = topic->get_document_references();
    if (drefs.size() > 0) {
        auto *dr_el = doc.NewElement("DocumentReferences");
        for (int i = 0; i < drefs.size(); i++) {
            Ref<BCFDocumentReference> dr = drefs[i];
            if (!dr.is_valid()) continue;
            auto *d = doc.NewElement("DocumentReference");
            d->SetAttribute("Guid", to_std(dr->get_guid()).c_str());
            if (!dr->get_document_guid().is_empty())
                add_text(doc, d, "DocumentGuid", to_std(dr->get_document_guid()));
            else if (!dr->get_url().is_empty())
                add_text(doc, d, "Url", to_std(dr->get_url()));
            add_text(doc, d, "Description", to_std(dr->get_description()));
            dr_el->InsertEndChild(d);
        }
        topic_el->InsertEndChild(dr_el);
    }

    // Related topics
    const Array &related = topic->get_related_topics();
    if (related.size() > 0) {
        auto *rt_el = doc.NewElement("RelatedTopics");
        for (int i = 0; i < related.size(); i++) {
            auto *rt = doc.NewElement("RelatedTopic");
            rt->SetAttribute("Guid", to_std(String(related[i])).c_str());
            rt_el->InsertEndChild(rt);
        }
        topic_el->InsertEndChild(rt_el);
    }

    // Comments
    const Array &comments = topic->get_comments();
    if (comments.size() > 0) {
        auto *comments_el = doc.NewElement("Comments");
        for (int i = 0; i < comments.size(); i++) {
            Ref<BCFComment> c = comments[i];
            if (!c.is_valid()) continue;
            auto *cel = doc.NewElement("Comment");
            cel->SetAttribute("Guid", to_std(c->get_guid()).c_str());
            add_text(doc, cel, "Date",           to_std(c->get_date()));
            // Author is required (NonEmptyOrBlankString, minOccurs=1); guard against empty
            {
                std::string ca = to_std(c->get_author());
                if (ca.empty()) ca = "editor";
                add_text(doc, cel, "Author", ca);
            }
            add_text(doc, cel, "Comment",        to_std(c->get_comment()));
            if (!c->get_viewpoint_guid().is_empty()) {
                auto *vp = doc.NewElement("Viewpoint");
                vp->SetAttribute("Guid", to_std(c->get_viewpoint_guid()).c_str());
                cel->InsertEndChild(vp);
            }
            add_text(doc, cel, "ModifiedDate",   to_std(c->get_modified_date()));
            add_text(doc, cel, "ModifiedAuthor", to_std(c->get_modified_author()));
            comments_el->InsertEndChild(cel);
        }
        topic_el->InsertEndChild(comments_el);
    }

    // Viewpoints (refs only in markup.bcf)
    const Array &viewpoints = topic->get_viewpoints();
    if (viewpoints.size() > 0) {
        auto *vps_el = doc.NewElement("Viewpoints");
        for (int i = 0; i < viewpoints.size(); i++) {
            Ref<BCFViewpointRef> vpr = viewpoints[i];
            if (!vpr.is_valid()) continue;
            auto *vp = doc.NewElement("ViewPoint");
            vp->SetAttribute("Guid", to_std(vpr->get_guid()).c_str());
            add_text(doc, vp, "Viewpoint", to_std(vpr->get_viewpoint_filename()));
            add_text(doc, vp, "Snapshot",  to_std(vpr->get_snapshot_filename()));
            if (vpr->get_index() >= 0) {
                auto *idx = doc.NewElement("Index");
                idx->SetText(vpr->get_index());
                vp->InsertEndChild(idx);
            }
            vps_el->InsertEndChild(vp);

            // Write the .bcfv file
            Ref<BCFVisualizationInfo> vis = vpr->get_visualization_info();
            if (vis.is_valid() && !vpr->get_viewpoint_filename().is_empty()) {
                std::string vp_xml = write_viewpoint(vis);
                zip.add_text_entry(folder + to_std(vpr->get_viewpoint_filename()), vp_xml);
            }

            // Write snapshot bytes
            const PackedByteArray &snap = vpr->get_snapshot_data();
            if (snap.size() > 0 && !vpr->get_snapshot_filename().is_empty()) {
                zip.add_entry(folder + to_std(vpr->get_snapshot_filename()),
                              std::vector<uint8_t>(snap.ptr(), snap.ptr() + snap.size()));
            }
        }
        topic_el->InsertEndChild(vps_el);
    }

    markup_el->InsertEndChild(topic_el);
    doc.InsertEndChild(markup_el);
    zip.add_text_entry(folder + "markup.bcf", doc_to_string(doc));
}

// ── VisualizationInfo → XML string ───────────────────────────────────────────

std::string BCFWriter::write_viewpoint(const Ref<BCFVisualizationInfo> &vis) {
    XMLDocument doc;
    doc.InsertEndChild(doc.NewDeclaration());
    auto *root = doc.NewElement("VisualizationInfo");
    root->SetAttribute("Guid", to_std(vis->get_guid()).c_str());

    // Components
    Ref<BCFComponents> comps = vis->get_components();
    if (comps.is_valid()) {
        auto *comp_el = doc.NewElement("Components");

        // Selection
        const Array &sel = comps->get_selection();
        if (sel.size() > 0) {
            auto *sel_el = doc.NewElement("Selection");
            for (int i = 0; i < sel.size(); i++) {
                Ref<BCFComponent> c = sel[i];
                if (c.is_valid()) write_component_el(doc, sel_el, c);
            }
            comp_el->InsertEndChild(sel_el);
        }

        // Visibility
        Ref<BCFComponentVisibility> cv = comps->get_visibility();
        if (cv.is_valid()) {
            auto *vis_el = doc.NewElement("Visibility");
            vis_el->SetAttribute("DefaultVisibility", cv->get_default_visibility());

            auto *hints = doc.NewElement("ViewSetupHints");
            hints->SetAttribute("SpacesVisible",          cv->get_spaces_visible());
            hints->SetAttribute("SpaceBoundariesVisible", cv->get_space_boundaries_visible());
            hints->SetAttribute("OpeningsVisible",        cv->get_openings_visible());
            vis_el->InsertEndChild(hints);

            const Array &exc = cv->get_exceptions();
            if (exc.size() > 0) {
                auto *exc_el = doc.NewElement("Exceptions");
                for (int i = 0; i < exc.size(); i++) {
                    Ref<BCFComponent> c = exc[i];
                    if (c.is_valid()) write_component_el(doc, exc_el, c);
                }
                vis_el->InsertEndChild(exc_el);
            }
            comp_el->InsertEndChild(vis_el);
        }

        // Coloring
        const Array &coloring = comps->get_coloring();
        if (coloring.size() > 0) {
            auto *col_el = doc.NewElement("Coloring");
            for (int i = 0; i < coloring.size(); i++) {
                Ref<BCFComponentColor> cc = coloring[i];
                if (!cc.is_valid()) continue;
                auto *c_el = doc.NewElement("Color");
                c_el->SetAttribute("Color", to_std(cc->get_color()).c_str());
                auto *comps_inner = doc.NewElement("Components");
                const Array &cc_comps = cc->get_components();
                for (int j = 0; j < cc_comps.size(); j++) {
                    Ref<BCFComponent> c = cc_comps[j];
                    if (c.is_valid()) write_component_el(doc, comps_inner, c);
                }
                c_el->InsertEndChild(comps_inner);
                col_el->InsertEndChild(c_el);
            }
            comp_el->InsertEndChild(col_el);
        }

        root->InsertEndChild(comp_el);
    }

    // Camera — xs:choice in visinfo.xsd is required (no minOccurs="0").
    // Always write a camera; fall back to a default perspective when none is set.
    int cam_type = vis->get_camera_type();
    if (cam_type == BCF_CAMERA_PERSPECTIVE) {
        Ref<BCFPerspectiveCamera> cam = vis->get_perspective_camera();
        auto *pc = doc.NewElement("PerspectiveCamera");
        if (cam.is_valid()) {
            add_vec3(doc, pc, "CameraViewPoint", cam->get_view_point());
            add_vec3(doc, pc, "CameraDirection",  cam->get_direction());
            add_vec3(doc, pc, "CameraUpVector",   cam->get_up_vector());
            auto *fov = doc.NewElement("FieldOfView"); fov->SetText(cam->get_fov()); pc->InsertEndChild(fov);
            auto *ar  = doc.NewElement("AspectRatio"); ar->SetText(cam->get_aspect_ratio()); pc->InsertEndChild(ar);
        } else {
            // Fallback identity perspective camera
            add_vec3(doc, pc, "CameraViewPoint", Vector3(0, 0, 0));
            add_vec3(doc, pc, "CameraDirection",  Vector3(0, 0, -1));
            add_vec3(doc, pc, "CameraUpVector",   Vector3(0, 1, 0));
            auto *fov = doc.NewElement("FieldOfView"); fov->SetText(60.0f); pc->InsertEndChild(fov);
            auto *ar  = doc.NewElement("AspectRatio"); ar->SetText(1.0f); pc->InsertEndChild(ar);
        }
        root->InsertEndChild(pc);
    } else if (cam_type == BCF_CAMERA_ORTHOGONAL) {
        Ref<BCFOrthogonalCamera> cam = vis->get_orthogonal_camera();
        auto *oc = doc.NewElement("OrthogonalCamera");
        if (cam.is_valid()) {
            add_vec3(doc, oc, "CameraViewPoint",  cam->get_view_point());
            add_vec3(doc, oc, "CameraDirection",  cam->get_direction());
            add_vec3(doc, oc, "CameraUpVector",   cam->get_up_vector());
            auto *s = doc.NewElement("ViewToWorldScale"); s->SetText(cam->get_view_to_world_scale()); oc->InsertEndChild(s);
            auto *ar = doc.NewElement("AspectRatio"); ar->SetText(cam->get_aspect_ratio()); oc->InsertEndChild(ar);
        } else {
            // Fallback identity orthogonal camera
            add_vec3(doc, oc, "CameraViewPoint",  Vector3(0, 0, 0));
            add_vec3(doc, oc, "CameraDirection",  Vector3(0, 0, -1));
            add_vec3(doc, oc, "CameraUpVector",   Vector3(0, 1, 0));
            auto *s = doc.NewElement("ViewToWorldScale"); s->SetText(100.0f); oc->InsertEndChild(s);
            auto *ar = doc.NewElement("AspectRatio"); ar->SetText(1.0f); oc->InsertEndChild(ar);
        }
        root->InsertEndChild(oc);
    } else {
        // BCF_CAMERA_NONE — write a default perspective camera to satisfy the required xs:choice
        auto *pc = doc.NewElement("PerspectiveCamera");
        add_vec3(doc, pc, "CameraViewPoint", Vector3(0, 0, 0));
        add_vec3(doc, pc, "CameraDirection",  Vector3(0, 0, -1));
        add_vec3(doc, pc, "CameraUpVector",   Vector3(0, 1, 0));
        auto *fov = doc.NewElement("FieldOfView"); fov->SetText(60.0f); pc->InsertEndChild(fov);
        auto *ar  = doc.NewElement("AspectRatio"); ar->SetText(1.0f); pc->InsertEndChild(ar);
        root->InsertEndChild(pc);
    }

    // Lines
    const Array &lines = vis->get_lines();
    if (lines.size() > 0) {
        auto *lines_el = doc.NewElement("Lines");
        for (int i = 0; i < lines.size(); i++) {
            Ref<BCFLine> l = lines[i];
            if (!l.is_valid()) continue;
            auto *le = doc.NewElement("Line");
            add_vec3(doc, le, "StartPoint", l->get_start_point());
            add_vec3(doc, le, "EndPoint",   l->get_end_point());
            lines_el->InsertEndChild(le);
        }
        root->InsertEndChild(lines_el);
    }

    // Clipping planes
    const Array &planes = vis->get_clipping_planes();
    if (planes.size() > 0) {
        auto *cp_el = doc.NewElement("ClippingPlanes");
        for (int i = 0; i < planes.size(); i++) {
            Ref<BCFClippingPlane> p = planes[i];
            if (!p.is_valid()) continue;
            auto *pe = doc.NewElement("ClippingPlane");
            add_vec3(doc, pe, "Location",  p->get_location());
            add_vec3(doc, pe, "Direction", p->get_direction());
            cp_el->InsertEndChild(pe);
        }
        root->InsertEndChild(cp_el);
    }

    // Bitmaps
    const Array &bitmaps = vis->get_bitmaps();
    if (bitmaps.size() > 0) {
        auto *bitmaps_el = doc.NewElement("Bitmaps");
        for (int i = 0; i < bitmaps.size(); i++) {
            Ref<BCFBitmap> b = bitmaps[i];
            if (!b.is_valid()) continue;
            auto *be = doc.NewElement("Bitmap");
            add_text(doc, be, "Format",    to_std(b->get_format()));
            add_text(doc, be, "Reference", to_std(b->get_reference()));
            add_vec3(doc, be, "Location",  b->get_location());
            add_vec3(doc, be, "Normal",    b->get_normal());
            add_vec3(doc, be, "Up",        b->get_up());
            auto *h = doc.NewElement("Height"); h->SetText(b->get_height()); be->InsertEndChild(h);
            bitmaps_el->InsertEndChild(be);
        }
        root->InsertEndChild(bitmaps_el);
    }

    doc.InsertEndChild(root);
    return doc_to_string(doc);
}
