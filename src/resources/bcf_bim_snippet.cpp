#include "bcf_bim_snippet.h"
using namespace godot;
void BCFBimSnippet::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_snippet_type", "v"),    &BCFBimSnippet::set_snippet_type);
    ClassDB::bind_method(D_METHOD("get_snippet_type"),         &BCFBimSnippet::get_snippet_type);
    ClassDB::bind_method(D_METHOD("set_is_external", "v"),     &BCFBimSnippet::set_is_external);
    ClassDB::bind_method(D_METHOD("get_is_external"),          &BCFBimSnippet::get_is_external);
    ClassDB::bind_method(D_METHOD("set_reference", "v"),       &BCFBimSnippet::set_reference);
    ClassDB::bind_method(D_METHOD("get_reference"),            &BCFBimSnippet::get_reference);
    ClassDB::bind_method(D_METHOD("set_reference_schema", "v"),&BCFBimSnippet::set_reference_schema);
    ClassDB::bind_method(D_METHOD("get_reference_schema"),     &BCFBimSnippet::get_reference_schema);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "snippet_type"),    "set_snippet_type",    "get_snippet_type");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL,   "is_external"),     "set_is_external",     "get_is_external");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "reference"),       "set_reference",       "get_reference");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "reference_schema"),"set_reference_schema","get_reference_schema");
}
