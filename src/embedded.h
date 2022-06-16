
/* This file is read by the build script "_comp.py" in order to generate
 * a compilation unit that embeds the desired content described here at each
 * compilation.
 * See "_comp.py" for more. */

#ifndef IPSYS_HEADER_EMBEDDED__
#define IPSYS_HEADER_EMBEDDED__

/* If this macro is used as would be the extern keayword in a variable
 * declaration, and if filename_ is a string literal, then the build
 * script "_comp.py" will parse the variable declaration as a special
 * declaration that requiers the variable to be set to the content of the
 * file at filename_ in the generated source file "embedded.c".
 * The variable declaration is expected to be in the global scope
 * and to have a type compatible with const char*. */
#define EMBEDDED(filename_) extern

EMBEDDED("shaders/particles.vert") char const* g_particles_vert;
EMBEDDED("shaders/particles.geom") char const* g_particles_geom;
EMBEDDED("shaders/particles.frag") char const* g_particles_frag;
EMBEDDED("shaders/ui_simple.vert") char const* g_ui_simple_vert;
EMBEDDED("shaders/ui_simple.frag") char const* g_ui_simple_frag;
EMBEDDED("shaders/gchars.vert") char const* g_gchars_vert;
EMBEDDED("shaders/gchars.geom") char const* g_gchars_geom;
EMBEDDED("shaders/gchars.frag") char const* g_gchars_frag;
EMBEDDED("shaders/fade.vert") char const* g_fade_vert;
EMBEDDED("shaders/fade.geom") char const* g_fade_geom;
EMBEDDED("shaders/fade.frag") char const* g_fade_frag;
EMBEDDED("shaders/texture_fade.vert") char const* g_texture_fade_vert;
EMBEDDED("shaders/texture_fade.geom") char const* g_texture_fade_geom;
EMBEDDED("shaders/texture_fade.frag") char const* g_texture_fade_frag;
EMBEDDED("shaders/texture_fade_2.vert") char const* g_texture_fade_2_vert;
EMBEDDED("shaders/texture_fade_2.frag") char const* g_texture_fade_2_frag;
EMBEDDED("shaders/iteruniv.comp") char const* g_iteruniv_comp;

#endif /* IPSYS_HEADER_EMBEDDED__ */
