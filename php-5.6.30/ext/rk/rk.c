#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdbool.h>

#include "php.h"
#include "rk.h"

static zend_function_entry rk_functions[] = {
    { NULL, NULL, NULL }
};

#ifdef DEBUG
#define RK_PRINTF(...) php_printf(__VA_ARGS__)
#else
#define RK_PRINTF(...)
#endif

static bool rk_hook(const char *method_name,
                    void (*hook)(INTERNAL_FUNCTION_PARAMETERS),
                    void (**original)(INTERNAL_FUNCTION_PARAMETERS))
{
  zend_function *function;

  if (!hook || !original) {
    RK_PRINTF("Couldn't hook, one of the functions is NULL");
    return false;
  }



  HashTable *ct = CG(class_table);

  int j = 0;
  Bucket buck = ct->arData[j];

  while (++j != ct->nNumOfElements)
    {
      if (!strcmp(buck.key->val, "pdo"))
        {
          RK_PRINTF("PDO found!\n");

          int i = 0;
          zend_class_entry *ft = buck.val.value.ce;

          function = ft->constructor;
          RK_PRINTF("ClassEntry: %s\n", ft->constructor->internal_function.function_name->val);

        }
      buck = ct->arData[j];
    }

	*original = function->internal_function.handler;
	function->internal_function.handler = *hook;
	RK_PRINTF("Successfully hooked function '%s' (original: %p) -> (hook: %p)\n", function->internal_function.function_name->val, &original, &hook);
}

void (*original)(zend_execute_data *, zval *);

PHP_FUNCTION(hooked_dbh_constructor)
{
  zval *object = getThis();
	char *data_source;
	int data_source_len;
	char *colon;
	char *username=NULL, *password=NULL;
	int usernamelen, passwordlen;
	zval *options = NULL;
	char alt_dsn[512];
	int call_factory = 1;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!s!a!", &data_source, &data_source_len,
				&username, &usernamelen, &password, &passwordlen, &options)) {
		ZVAL_NULL(object);
		return;
	}

  php_printf("Hook was called with parameters: %s:%s\nNow calling the original function...\n\n", username, password);

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
