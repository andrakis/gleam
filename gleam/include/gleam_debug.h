#ifndef _GLEAM_DEBUG_H
#define _GLEAM_DEBUG_H

#include "gleam.h"

#ifndef NDEBUG
	#define GLEAM_DBG(from, add, to, val) gleam_debug(from, add, to, val)
#else
	#define GLEAM_DBG(from, add, to, val)
#endif

void gleam_debug(gnum from, gnum add, gnum to, gnum val);

#endif
