#pragma once

#include <QtCore/qglobal.h>

#ifndef VISION_CORE_BUILD_STATIC
# if defined(VISION_CORE_LIB)
#  define VISION_CORE_EXPORT Q_DECL_EXPORT
# else
#  define VISION_CORE_EXPORT Q_DECL_IMPORT
# endif
#else
# define VISION_CORE_EXPORT
#endif
