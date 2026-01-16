#pragma once

#include <QtCore/qglobal.h>

#ifndef ZYHN_DATA_BUILD_STATIC
# if defined(ZYHN_DATA_LIB)
#  define ZYHN_DATA_EXPORT Q_DECL_EXPORT
# else
#  define ZYHN_DATA_EXPORT Q_DECL_IMPORT
# endif
#else
# define ZYHN_DATA_EXPORT
#endif
