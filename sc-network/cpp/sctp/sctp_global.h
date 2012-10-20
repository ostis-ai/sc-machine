#ifndef SCTP_GLOBAL_H
#define SCTP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SCTP_LIBRARY)
#  define SCTPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SCTPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SCTP_GLOBAL_H
