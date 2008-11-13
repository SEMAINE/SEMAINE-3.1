#ifndef _HPTS_DLL_H
#define _HPTS_DLL_H

#ifdef _WIN32

#pragma warning(disable: 4251)  // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable: 4275)  // non DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning( disable : 4503 )	// decorated name length exceeded, name was truncated
#pragma warning( disable : 4661 )	// no suitable definition provided for explicit template instantiation request
#pragma warning( disable : 4786 )	// identifier was truncated to '255' characters in the debug information
#pragma warning(disable : 4129) // '.' : unrecognized character escape sequence

#ifndef HPTS_STATIC

#ifdef HPTS_DLL_EXPORT

#define HPTS_DLL __declspec(dllexport)

#else

#define HPTS_DLL __declspec(dllimport)

#endif

#else

#define HPTS_DLL 

#endif

#endif

#endif

#ifndef _WIN32

#define HPTS_DLL

#endif
