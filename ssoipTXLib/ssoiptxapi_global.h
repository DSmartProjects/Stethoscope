#ifndef SOIPTXS_GLOBAL_H
#define SOIPTXS_GLOBAL_H


#ifdef SOIPTXS_LIB
# define SOIPTXS_EXPORT __declspec(dllexport)
#else
# define SOIPTXS_EXPORT __declspec(dllimport)
#endif

#endif // SOIPTXS_GLOBAL_H
