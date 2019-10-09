#ifndef A3D_GLOBAL_H
#define A3D_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(A3D_LIBRARY)
#  define A3DSHARED_EXPORT Q_DECL_EXPORT
#else
#  define A3DSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // A3D_GLOBAL_H
