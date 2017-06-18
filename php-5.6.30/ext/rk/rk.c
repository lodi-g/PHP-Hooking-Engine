#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdbool.h>

#include "php.h"
#include "rk.h"

static zend_function_entry rk_functions[] = {
    { NULL, NULL, NULL }
};

static bool rk_hook(const char *method_name,
                    void (*hook)(INTERNAL_FUNCTION_PARAMETERS),
                    void (**original)(INTERNAL_FUNCTION_PARAMETERS))
{
  zend_function *function;

  if (!hook || !original) {
    php_printf("Couldn't hook, one of the functions is NULL");
    return false;
  }



  HashTable *ft = CG(function_table);
  int i = ft->nNumOfElements;
  uint32_t ip = ft->nInternalPointer;

  php_printf("There is %d functions in the function table\n", i);

  int j = 0;
  Bucket buck = ft->arData[j];

  while (i--)
    {
      php_printf("=> '%s'\n", buck.key->val);
      buck = ft->arData[++j];
    }


  if ((function = zend_hash_str_find_ptr(CG(function_table), method_name, strlen(method_name))) != NULL) {
		*original = function->internal_function.handler;
		function->internal_function.handler = *hook;
		php_printf("Successfully hooked function '%s' (original: %p) -> (hook: %p)\n", method_name, &original, &hook);
		return true;
	} else {
		php_printf("Unable to locate function '%s' in global function table.\n", method_name);
		return false;
	}
}

void (*original)(zend_execute_data *, zval *);

PHP_FUNCTION(hooked_dbh_constructor)
{
	php_printf("Hook was called!\n");
  original(execute_data, return_value);
}

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)
#pragma message VAR_NAME_VALUE(PHP_FUNCTION(hooked_dbh_constructor))

PHP_MINIT_FUNCTION(rk)
{
  rk_hook("zend_version", zif_hooked_dbh_constructor, &original);
  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(rk)
{
  return SUCCESS;
}

zend_module_entry rk_module_entry = {
  STANDARD_MODULE_HEADER,
  RK_NAME,
  NULL, /* No public functions */
  PHP_MINIT(rk),
  PHP_MSHUTDOWN(rk),
  NULL,
  NULL,
  NULL,
  RK_VERSION,
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_RK
ZEND_GET_MODULE(rk)
#endif
