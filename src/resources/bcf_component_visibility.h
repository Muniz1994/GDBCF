#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFComponentVisibility : public Resource {
    GDCLASS(BCFComponentVisibility, Resource)

    bool  default_visibility     = false;
    Array exceptions;             // Array[BCFComponent]
    bool  spaces_visible          = false;
    bool  space_boundaries_visible= false;
    bool  openings_visible        = false;

protected:
    static void _bind_methods();

public:
    void set_default_visibility(bool v)      { default_visibility = v; }
    bool get_default_visibility() const      { return default_visibility; }

    void set_exceptions(const Array &v)      { exceptions = v; }
    Array get_exceptions() const             { return exceptions; }

    void set_spaces_visible(bool v)          { spaces_visible = v; }
    bool get_spaces_visible() const          { return spaces_visible; }

    void set_space_boundaries_visible(bool v){ space_boundaries_visible = v; }
    bool get_space_boundaries_visible() const{ return space_boundaries_visible; }

    void set_openings_visible(bool v)        { openings_visible = v; }
    bool get_openings_visible() const        { return openings_visible; }
};
