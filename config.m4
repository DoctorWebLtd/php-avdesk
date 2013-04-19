dnl config.m4 for extension dwavdapi

PHP_ARG_WITH(dwavd, for Dr.Web AV-Desk support, [  --with-dwavd=[DIR]     Include Dr.Web AV-Desk support])

if test "$PHP_DWAVD" != "no"; then
    AC_MSG_CHECKING(for PHP 5.3.0 or greater)

    PHP_VER_NUM=0

    if test -z "$PHP_MAJOR_VERION"; then
        PHP_CONFIG=`which php-config`
	if test -z "$PHP_CONFIG"; then
    	     AC_MSG_ERROR(unable to check PHP version because php-config not found)
        fi
	PHP_VER_NUM=`$PHP_CONFIG --vernum`
    else
	PHP_NUM_VERSION=`expr [$]PHP_MAJOR_VERSION \* 10000 + [$]PHP_MINOR_VERSION \* 100 + [$]PHP_RELEASE_VERSION`
    fi

    if test "$PHP_VER_NUM" -ge 50300; then
	AC_MSG_RESULT($PHP_VER_NUM)
    else
	AC_MSG_ERROR(PHP version 5.3.0 or later is required)
    fi
  
  dnl checking header file
  SEARCH_HEADERS_PATH="/usr/local /usr /opt/local"     
  SEARCH_HEADER_FILE="avdesk.h"  
   if test -r $PHP_AVDESK/include/$SEARCH_HEADER_FILE; then
     DWAVDAPI_DIR=$PHP_AVDESK
   else
      AC_MSG_CHECKING([for include/dwavdapi/avdesk.h in default path])
      for i in $SEARCH_HEADERS_PATH; do
        if test -r $i/include/dwavdapi/$SEARCH_HEADER_FILE; then
           DWAVDAPI_DIR=$i
           AC_MSG_RESULT(found in $i)
           break
        fi
      done
  fi
  
   if test -z "$DWAVDAPI_DIR"; then
     AC_MSG_RESULT([not found])
     AC_MSG_ERROR([Please reinstall the libdwavdapi distribution - avdesk.h should be in <dwavdapi-dir>/include/dwavdapi/)])
   fi

  DWAVDAPI_CONFIG=""
  AC_MSG_CHECKING(for dwavdapi 2.1.0 or greater)
  
  if ${DWAVDAPI_DIR}/bin/dwavdapi-config --libs > /dev/null 2>&1; then
    DWAVDAPI_CONFIG=${DWAVDAPI_DIR}/bin/dwavdapi-config
  else
    if ${DWAVDAPI_DIR}/dwavdapi-config --libs > /dev/null 2>&1; then
      DWAVDAPI_CONFIG=${DWAVDAPI_DIR}/dwavdapi-config
    fi
  fi
  
  if test "x$DWAVDAPI_CONFIG" = "x"; then
     AC_MSG_ERROR(dwavdapi library version 2.1.0 or later is required to compile with Dr.Web AV-Desk support)
  fi
  
  DWAVDAPI_VERSION_NUM=`$DWAVDAPI_CONFIG --vernum`
  
  if test "$DWAVDAPI_VERSION_NUM" -ge 20100; then
    DWAVDAPI_VERSION=`$DWAVDAPI_CONFIG --version`
    AC_MSG_RESULT($DWAVDAPI_VERSION)
    DWAVDAPI_LIBS=`$DWAVDAPI_CONFIG --libs`
  else
    AC_MSG_ERROR(dwavdapi library version 2.1.0 or later is required to compile with Dr.Web AV-Desk support)
  fi
  
  AC_CHECK_HEADERS([stdarg.h], [AC_DEFINE([HAVE_STDARG_H], [1], [Define to 1 if you have the <stdarg.h> header file])])
  AC_CHECK_HEADERS([time.h], [AC_DEFINE([HAVE_TIME_H], [1], [Define to 1 if you have the <time.h> header file])])
    
  LIB_NAME=dwavdapi
  PHP_ADD_INCLUDE($DWAVDAPI_DIR/include)
  PHP_ADD_INCLUDE($DWAVDAPI_DIR/include/dwavdapi)
  PHP_EVAL_LIBLINE("$DWAVDAPI_LIBS", DWAVD_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH($LIB_NAME, "", DWAVD_SHARED_LIBADD)
  
   PHP_SUBST(DWAVD_SHARED_LIBADD)
   PHP_NEW_EXTENSION( dwavd, php_utils.c dwavd.c, $ext_shared)
fi
