PHP_ARG_ENABLE(rk, whether to enable rk support, [ --enable-rk   Enable rk support])
if test "$PHP_RK" = "yes"; then
  AC_DEFINE(HAVE_RK, 1, [Whether you have rk])
  PHP_NEW_EXTENSION(rk, rk.c, $ext_shared)
fi
