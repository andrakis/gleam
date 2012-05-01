/**
 * Platform-specific header
 */

#ifdef _MSC_VER
	#include <conio.h>

	// atoll is _atoi64
	#define atoll _atoi64
#else
	#define getch()
#endif
