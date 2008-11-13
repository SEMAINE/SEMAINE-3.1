#include <exception>

#ifndef _MACRO_EXCEPTIONS_
#define _MACRO_EXCEPTIONS_

// Macros definitions
// to handle exceptions 

//
// defines exception A derivating from exception B
// containing message C
//
#define EXCEPTION(A,B,C) \
class A : public B       \
{                        \
 public:                 \
  const char * what() const throw ()    \
    { return C ; }       \
}                  


#ifndef _NO_VERIFY
//
// A : expression
// B : exception class
// if A is not verified, B is thrown
//
#define VERIFY(A,B) \
if(!(A))            \
{                   \
  *(char*)0 = 0 ;   \
  throw B() ;       \
}

#else
#define VERIFY(A,B)
#endif

#endif


