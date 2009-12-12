dnl $Id$
dnl config.m4 for extension pasic

PHP_ARG_ENABLE(pasic, whether to enable pasic support,
[  --enable-pasic           Enable pasic support])

if test "$PHP_PASIC" != "no"; then
  PHP_NEW_EXTENSION(pasic, pasic.c, $ext_shared)
fi
