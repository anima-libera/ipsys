
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

EMBEDDED("shaders/particles.vert") const char* g_particles_vert;
EMBEDDED("shaders/particles.geom") const char* g_particles_geom;
EMBEDDED("shaders/particles.frag") const char* g_particles_frag;
EMBEDDED("shaders/ui_simple.vert") const char* g_ui_simple_vert;
EMBEDDED("shaders/ui_simple.frag") const char* g_ui_simple_frag;
EMBEDDED("shaders/fade.vert") const char* g_fade_vert;
EMBEDDED("shaders/fade.geom") const char* g_fade_geom;
EMBEDDED("shaders/fade.frag") const char* g_fade_frag;
EMBEDDED("shaders/texture_fade.vert") const char* g_texture_fade_vert;
EMBEDDED("shaders/texture_fade.geom") const char* g_texture_fade_geom;
EMBEDDED("shaders/texture_fade.frag") const char* g_texture_fade_frag;
EMBEDDED("shaders/iteruniv.comp") const char* g_iteruniv_comp;

#endif /* IPSYS_HEADER_EMBEDDED__ */
