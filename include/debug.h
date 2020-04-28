#ifndef _DEBUG_H_
#define _DEBUG_H_

#define PRINT_INT(n) printf("%d\n", n)
#define PRINT_SIZE_T(n) printf("%lu\n", n)
#define PRINT_STR(s) printf("%s\n", s)

#define PRINT_INT_VAR(n) printf("%s = %d\n", #n, n)
#define PRINT_SIZE_T_VAR(n) printf("%s = %lu\n", #n, n)
#define PRINT_STR_VAR(s) printf("%s = %s\n", #s, s)

#endif
