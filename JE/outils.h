#ifndef _OUTILS_H
#define _OUTILS_H

#ifndef BIT
#   define BIT(b) (1<<(b))
#endif

#ifndef MIN
#   define MIN(a, b) ((a)>(b)?(b):(a))
#endif

#ifndef MAX
#   define MAX(a, b) ((a)>(b)?(a):(b))
#endif

#ifndef CLAMP
#   define CLAMP(m, n, M) MIN(M, MAX(m, n))
#endif

#endif

