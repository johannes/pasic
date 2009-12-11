dnl $Id$
dnl config.m4 for extension basic

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(basic, for basic support,
dnl Make sure that the comment is aligned:
dnl [  --with-basic             Include basic support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(basic, whether to enable basic support,
dnl Make sure that the comment is aligned:
dnl [  --enable-basic           Enable basic support])

if test "$PHP_BASIC" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-basic -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/basic.h"  # you most likely want to change this
  dnl if test -r $PHP_BASIC/$SEARCH_FOR; then # path given as parameter
  dnl   BASIC_DIR=$PHP_BASIC
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for basic files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       BASIC_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$BASIC_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the basic distribution])
  dnl fi

  dnl # --with-basic -> add include path
  dnl PHP_ADD_INCLUDE($BASIC_DIR/include)

  dnl # --with-basic -> check for lib and symbol presence
  dnl LIBNAME=basic # you may want to change this
  dnl LIBSYMBOL=basic # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $BASIC_DIR/lib, BASIC_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_BASICLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong basic lib version or lib not found])
  dnl ],[
  dnl   -L$BASIC_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(BASIC_SHARED_LIBADD)

  PHP_NEW_EXTENSION(basic, basic.c, $ext_shared)
fi
