// #include <stdint.h>
// #include <stdio.h>
// #include <string.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "lib/utils/pyexec.h"

#include "uart.h"

/* fill in __assert_fail for libc */
void __assert_fail(const char *__assertion, const char *__file,
            unsigned int __line, const char *__function)
{
    printk("Assert at %s:%d:%s() \"%s\" failed\n", __file, __line, __function, __assertion);
    for (;;) {;
    }
}

int raise (int signum)
{
	printk("raise: Signal # %d caught\n", signum);
	return 0;
}

extern void s3c2440_led_init(void);

int main(int argc, char **argv)
{
    void *heap_start = (void *)0x31000000;
    unsigned int heap_len = 0x100000;

    s3c2440_uart0_init();
    s3c2440_led_init();
    
    #if MICROPY_ENABLE_GC
    gc_init(heap_start, heap_start + heap_len);
    #endif
 
    mp_init();

    pyexec_friendly_repl();

    mp_deinit();

    while (1);
    return 0;
}

#if 1

#if MICROPY_ENABLE_GC
void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    static char *stack_top;
    
    volatile void *tmp = (void *)0x32000000;
    volatile void *sp = &tmp;
 
    gc_collect_start();
    gc_collect_root((void**)sp, ((mp_uint_t)0x32000000 - (mp_uint_t)0x31000000) / sizeof(mp_uint_t));
    gc_collect_end();
    // gc_dump_info();
}
#endif

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
	printk("mp_lexer_new_from_file\n");
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
	printk("mp_import_stat\n");
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
	printk("mp_builtin_open\n");
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    while (1) {
        ;
    }
}

void NORETURN __fatal_error(const char *msg) {
    while (1) {
        ;
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printk("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

#include <stdarg.h>
int mp_vprintf(const mp_print_t *print, const char *fmt, va_list args);
int DEBUG_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = mp_vprintf(MICROPY_DEBUG_PRINTER, fmt, ap);
    va_end(ap);
    return ret;
}

// Send "cooked" string of given length, where every occurrence of
// LF character is replaced with CR LF.
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    while (len--) {
        printk("%c", *str++);
    }
}

// Send zero-terminated string
void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}
#endif
