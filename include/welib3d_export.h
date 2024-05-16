
#ifndef WELIB3D_EXPORT_H
#define WELIB3D_EXPORT_H

#ifdef WELIB3D_STATIC_DEFINE
#  define WELIB3D_EXPORT
#  define WELIB3D_NO_EXPORT
#else
#  ifndef WELIB3D_EXPORT
#    ifdef welib3d_EXPORTS
        /* We are building this library */
#      define WELIB3D_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define WELIB3D_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef WELIB3D_NO_EXPORT
#    define WELIB3D_NO_EXPORT 
#  endif
#endif

#ifndef WELIB3D_DEPRECATED
#  define WELIB3D_DEPRECATED __declspec(deprecated)
#endif

#ifndef WELIB3D_DEPRECATED_EXPORT
#  define WELIB3D_DEPRECATED_EXPORT WELIB3D_EXPORT WELIB3D_DEPRECATED
#endif

#ifndef WELIB3D_DEPRECATED_NO_EXPORT
#  define WELIB3D_DEPRECATED_NO_EXPORT WELIB3D_NO_EXPORT WELIB3D_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef WELIB3D_NO_DEPRECATED
#    define WELIB3D_NO_DEPRECATED
#  endif
#endif

#endif /* WELIB3D_EXPORT_H */
