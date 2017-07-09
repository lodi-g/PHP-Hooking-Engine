PHP_ARG_ENABLE(he, whether to enable he support, [ --enable-he   Enable he support])
if test "$PHP_HE" = "yes"; then
  AC_DEFINE(HAVE_HE, 1, [Whether you have he])
  PHP_NEW_EXTENSION(he, he.c, $ext_shared)
fi
