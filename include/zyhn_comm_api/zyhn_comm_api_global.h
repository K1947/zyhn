#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ZYHN_COMM_API_LIB)
#  define ZYHN_COMM_API_EXPORT Q_DECL_EXPORT
# else
#  define ZYHN_COMM_API_EXPORT Q_DECL_IMPORT
# endif
#else
# define ZYHN_COMM_API_EXPORT
#endif