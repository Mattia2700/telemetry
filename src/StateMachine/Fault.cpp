#include "StateMachine/Fault.h"


//----------------------------------------------------------------------------
// FaultHandler
//----------------------------------------------------------------------------
void FaultHandler(const char* file, unsigned short line)
{
	// If you hit this line, it means one of the ASSERT macros failed.
#if WIN32
    DebugBreak();

	assert(0);
#endif

	// Trap fault here
	while(1);
}
