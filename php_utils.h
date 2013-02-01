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


#ifndef _PHP_UTILS_H_
#define	_PHP_UTILS_H_

#ifdef	__cplusplus
extern "C" {
#endif

void _dwavd_error(int type, const char *format, ...);
const char *_dwavd_var_type(zval *var);
void _dwavd_carray_to_phparray(zval **php_array, const char **carray, unsigned int carray_size);


#ifdef	__cplusplus
}
#endif

#endif	/* _PHP_UTILS_H_ */
