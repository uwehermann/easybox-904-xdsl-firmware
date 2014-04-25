
#if defined(__ARCH_BIG_ENDIAN__) || (__BYTE_ORDER == __BIG_ENDIAN)
  #define BREAKPOINT_VALUE { 0x00, 0x00, 0x00, 0x0d }
#endif

#if defined(__ARCH_LITTLE_ENDIAN__) || (__BYTE_ORDER == __LITTLE_ENDIAN)
  #define BREAKPOINT_VALUE { 0x0d, 0x00, 0x00, 0x00 }
#endif

#ifndef BREAKPOINT_VALUE
  #error BREAKPOINT_VALUE not defined
#endif

#define BREAKPOINT_LENGTH 4
#define DECR_PC_AFTER_BREAK 0

#define LT_ELFCLASS	ELFCLASS32
#define LT_ELF_MACHINE	EM_MIPS

#define PLTs_INIT_BY_HERE "_start"
#define E_ENTRY_NAME    "_start"
