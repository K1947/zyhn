#pragma once

#include <QtCore/qglobal.h>

#ifndef ZYHN_VISION_EXPORT
# if defined(ZYHN_VISION_LIB)
#  define ZYHN_VISION_EXPORT Q_DECL_EXPORT
# else
#  define ZYHN_VISION_EXPORT Q_DECL_IMPORT
# endif
#else
# define ZYHN_VISION_EXPORT
#endif
