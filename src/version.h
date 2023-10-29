#ifndef _VERSION_H_
#define _VERSION_H_

#include <string>

//static const std::string version("0.0.2"); //doesn't work with SWIG
inline std::string GetVersion() { return "0.0.5"; }

#endif//_VERSION_H_
