#pragma once

#include <QtCore/qglobal.h>

#ifndef IMAGE_EDITOR_BUILD_STATIC
# if defined(IMAGE_EDITOR_LIB)
#  define IMAGE_EDITOR_EXPORT Q_DECL_EXPORT
# else
#  define IMAGE_EDITOR_EXPORT Q_DECL_IMPORT
# endif
#else
# define IMAGE_EDITOR_EXPORT
#endif
