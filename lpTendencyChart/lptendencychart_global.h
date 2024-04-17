#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LPTENDENCYCHART_LIB)
#  define LPTENDENCYCHART_EXPORT Q_DECL_EXPORT
# else
#  define LPTENDENCYCHART_EXPORT Q_DECL_IMPORT
# endif
#else
# define LPTENDENCYCHART_EXPORT
#endif
