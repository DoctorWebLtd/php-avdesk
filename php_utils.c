/*
 * Copyright (c) Doctor Web, 2003-2011
 *
 * Following code is the property of Doctor Web, Ltd.
 * Dr.Web is a registred trademark of Doctor Web, Ltd.
 *
 * http://www.drweb.com
 * http://www.av-desk.com
 *
 */

#include <stdlib.h>
#include <errno.h>

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <Zend/zend.h>
#include <Zend/zend_globals.h>
#include <Zend/zend_API.h>

void _dwavd_error(int type, const char *format, ...) {
    char buff[1024];
    char *space = NULL;
    char *class_name = NULL;
    va_list args;
    
    class_name = get_active_class_name(&space TSRMLS_CC);
    snprintf(&buff[0], sizeof (buff) - 1, "%s%s%s(): ", class_name, space, get_active_function_name(TSRMLS_C));
    
    va_start(args, format);
    vsnprintf(&buff[strlen(buff)], sizeof(buff)-strlen(buff)-1, format, args);
    va_end(args);
    
    zend_error(type, "%s", buff);
}

void _dwavd_carray_to_phparray(zval **php_array, const char **carray, unsigned int carray_size) {
    unsigned int i = 0;
    zval *array = NULL;
    MAKE_STD_ZVAL(array)
    array_init(array);
    for(i = 0; i< carray_size; i++) {
        add_index_string(array, i, carray[i], 1);
    }
    *php_array = array;
}

const char *_dwavd_var_type(zval *var) {
    switch (Z_TYPE_P(var)) {
        case IS_BOOL:
            return Z_BVAL_P(var) ? "true" : "false";
            break;
        case IS_LONG:
            return "long";
            break;
        case IS_DOUBLE:
            return "double";
            break;
        case IS_STRING:
            return "string";
            break;
        case IS_ARRAY:
            return "array";
            break;
        case IS_NULL:
            return "null";
            break;
        case IS_RESOURCE:
            return "resource";
            break;
        case IS_OBJECT:
            return "object";
            break;
        default:
            return "<unknown>";
    }
}