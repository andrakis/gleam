/**
 * Platform-specific header
 */

#ifdef WINDOWS
	#include <conio.h>
#else
	#define getch()
#endif
