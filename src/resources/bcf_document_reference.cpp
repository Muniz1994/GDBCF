#include "bcf_document_reference.h"
using namespace godot;
void BCFDocumentReference::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_guid", "v"),          &BCFDocumentReference::set_guid);
    ClassDB::bind_method(D_METHOD("get_guid"),               &BCFDocumentReference::get_guid);
    ClassDB::bind_method(D_METHOD("set_document_guid", "v"), &BCFDocumentReference::set_document_guid);
    ClassDB::bind_method(D_METHOD("get_document_guid"),      &BCFDocumentReference::get_document_guid);
    ClassDB::bind_method(D_METHOD("set_url", "v"),           &BCFDocumentReference::set_url);
    ClassDB::bind_method(D_METHOD("get_url"),                &BCFDocumentReference::get_url);
    ClassDB::bind_method(D_METHOD("set_description", "v"),   &BCFDocumentReference::set_description);
    ClassDB::bind_method(D_METHOD("get_description"),        &BCFDocumentReference::get_description);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "guid"),          "set_guid",          "get_guid");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "document_guid"), "set_document_guid", "get_document_guid");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "url"),           "set_url",           "get_url");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"),   "set_description",   "get_description");
}
