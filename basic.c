/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 252479 2008-02-07 19:39:50Z iliaa $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_basic.h"

/* {{{ proto string basic_compile(string function_name, string filename)
   compile filename into function function_name */
PHP_FUNCTION(basic_compile)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 *  */
PHP_MINFO_FUNCTION(basic)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "basic support", "enabled");
	php_info_print_table_end();
}   
/* }}} */

/* {{{ basic_functions[]
 */
const zend_function_entry basic_functions[] = {
	PHP_FE(basic_compile,	NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ basic_module_entry
 */
zend_module_entry basic_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"basic",
	basic_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(basic),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1",
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_BASIC
ZEND_GET_MODULE(basic)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
