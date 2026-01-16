#pragma once

#include <QtCore/qglobal.h>

#ifndef ZYHN_API_BUILD_STATIC
# if defined(ZYHN_API_LIB)
#  define ZYHN_API_EXPORT Q_DECL_EXPORT
# else
#  define ZYHN_API_EXPORT Q_DECL_IMPORT
# endif
#else
# define ZYHN_API_EXPORT
#endif