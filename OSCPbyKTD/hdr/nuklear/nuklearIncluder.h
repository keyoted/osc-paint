#ifndef nuklearIncluder_h
#define nuklearIncluder_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include <SFML/Window.hpp>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SFML_GL3_IMPLEMENTATION

#ifdef IS_IMPLEMENTATION
	#include "nuklear.h"
	#include "nuklear_sfml_gl3.h"
#endif


#define SIZEMULTIPLIER 190
#define WINDOW_WIDTH 6 * SIZEMULTIPLIER
#define WINDOW_HEIGHT 4 * SIZEMULTIPLIER

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#endif
