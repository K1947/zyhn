#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ZYHN_CONTROL_LIB)
#  define ZYHN_CONTROL_EXPORT Q_DECL_EXPORT
# else
#  define ZYHN_CONTROL_EXPORT Q_DECL_IMPORT
# endif
#else
# define ZYHN_CONTROL_EXPORT
#endif
