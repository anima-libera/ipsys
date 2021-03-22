
/* APSILG header file "srcinsert.h"
 * This file provides an interface to the content inserted by the compilation
 * script "_comp.py" into the source code by generating the file "srcinsert.c"
 * at each compilation.
 * See "_compile.py" and SOURCE_INSERTION for more. */

#ifndef __APSILG_header_srcinsert__
#define __APSILG_header_srcinsert__

/* Each SOURCE_INSERTION(file, vardecl) triggers the generation of the code
 * `vardecl = "filecontent";` in the generated file "srcinsert.c" each time the
 * compilation script "_comp.py" runs. In the generated code, filecontent is
 * replaced by the content of the given file with some escaped characters so
 * that everything goes as if the content of the given file was assigned to the
 * variable declared in vardecl.
 * Here is an example:
 * SOURCE_INSERTION("iter.comp", const char* g_srcinserted_iter_comp);
 * Note that the line must SOURCE_INSERTION must be at the beginning of the
 * line and that there should not be unnecessary spaces anywhere except in the
 * vardecl and after the separating comma. The file parameter must be a string
 * litteral and the vardecl must be a variable declaration without the ending
 * semicolon. They must not require any preprocessor job to be expanded.
 * The file parameter should not contain any escaped character.
 * Please just do as in the example, everything is gonna be alright <(^^)v. */
#define SOURCE_INSERTION(file, vardecl) extern vardecl

SOURCE_INSERTION("particles.vert", const char* g_srcinserted_particles_vert);
SOURCE_INSERTION("particles.geom", const char* g_srcinserted_particles_geom);
SOURCE_INSERTION("particles.frag", const char* g_srcinserted_particles_frag);
SOURCE_INSERTION("fade.vert", const char* g_srcinserted_fade_vert);
SOURCE_INSERTION("fade.geom", const char* g_srcinserted_fade_geom);
SOURCE_INSERTION("fade.frag", const char* g_srcinserted_fade_frag);
SOURCE_INSERTION("iteruniv.comp", const char* g_srcinserted_iteruniv_comp);

#endif /* __APSILG_header_srcinsert__ */
