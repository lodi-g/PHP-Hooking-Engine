#ifndef HE_H
#define HE_H

#define HE_VERSION "1.0"
#define HE_NAME "Hooking Engine"

/* Debugging macro - uncomment the following line to enable it */
/* #define DEBUG */
#ifdef DEBUG
#define HE_PRINTF(...) php_printf(__VA_ARGS__)
#else
#define HE_PRINTF(...)
#endif

extern zend_module_entry he_module_entry;
#define phpext_he_ptr &he_module_entry

#endif /* !HE_H */
