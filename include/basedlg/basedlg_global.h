#pragma once

#include <QtCore/qglobal.h>

#ifndef BASEDLG_BUILD_STATIC
# if defined(BASEDLG_LIB)
#  define BASEDLG_EXPORT Q_DECL_EXPORT
# else
#  define BASEDLG_EXPORT Q_DECL_IMPORT
# endif
#else
# define BASEDLG_EXPORT
#endif
