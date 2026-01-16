#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ZYHN_CAMERA_LIB)
#  define ZYHN_CAMERA_EXPORT Q_DECL_EXPORT
# else
#  define ZYHN_CAMERA_EXPORT Q_DECL_IMPORT
# endif
#else
# define ZYHN_CAMERA_EXPORT
#endif
