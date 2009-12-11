dnl $Id$
dnl config.m4 for extension basic

PHP_ARG_ENABLE(basic, whether to enable basic support,
[  --enable-basic           Enable basic support])

if test "$PHP_BASIC" != "no"; then
  PHP_NEW_EXTENSION(basic, basic.c, $ext_shared)
fi
