#include <pololu/3pi.h>
#include "orienteering.h"

// This is the main function, where the code starts.  All C programs
// must have a main() function defined somewhere.
#ifdef AVR_3PI
int main()
{
   startup();
   // We should never reach this point..
   return 0;
}
#endif
