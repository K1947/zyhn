#pragma once

#include <QtCore/qglobal.h>

#ifndef ZYHN_SPC_BUILD_STATIC
# if defined(ZYHN_SPC_LIB)
#  define ZYHN_SPC_EXPORT Q_DECL_EXPORT
# else
#  define ZYHN_SPC_EXPORT Q_DECL_IMPORT
# endif
#else
# define ZYHN_SPC_EXPORT
#endif
