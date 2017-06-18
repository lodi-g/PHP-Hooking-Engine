#ifndef RK_H
#define RK_H

#define RK_VERSION "1.0"
#define RK_NAME "rk"

extern PHP_FUNCTION(hash);

extern zend_module_entry rk_module_entry;
#define phpext_rk_ptr &rk_module_entry

#endif
