#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// BCF Bitmap: an image overlay placed in 3D space inside a viewpoint.
// format: "png" or "jpg" as per BCF spec.
// reference: filename inside the topic folder.
class BCFBitmap : public Resource {
    GDCLASS(BCFBitmap, Resource)

    String  format;       // "png" or "jpg"
    String  m_reference;  // filename in topic folder
    Vector3 location;
    Vector3 normal;
    Vector3 up;
    double  height = 1.0;

protected:
    static void _bind_methods();

public:
    void set_format(const String &v)    { format = v; }
    String get_format() const           { return format; }
    void set_reference(const String &v) { m_reference = v; }
    String get_reference() const        { return m_reference; }
    void set_location(const Vector3 &v) { location = v; }
    Vector3 get_location() const        { return location; }
    void set_normal(const Vector3 &v)   { normal = v; }
    Vector3 get_normal() const          { return normal; }
    void set_up(const Vector3 &v)       { up = v; }
    Vector3 get_up() const              { return up; }
    void set_height(double v)           { height = v; }
    double get_height() const           { return height; }
};
