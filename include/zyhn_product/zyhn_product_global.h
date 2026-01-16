#pragma once

#ifndef ZYHN_PRODUCT_BUILD_STATIC
# if defined(ZYHN_PRODUCT_LIB)
#  define ZYHN_PRODUCT_EXPORT Q_DECL_EXPORT
# else
#  define ZYHN_PRODUCT_EXPORT Q_DECL_IMPORT
# endif
#else
# define ZYHN_PRODUCT_EXPORT
#endif