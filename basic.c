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

static int lookup_cv(zend_op_array *op_array, char *name, int name_len)
{
	int i = 0;
	ulong hash_value = zend_inline_hash_func(name, name_len+1);

	while (i < op_array->last_var) {
		if (op_array->vars[i].hash_value == hash_value &&
			op_array->vars[i].name_len == name_len &&
			!memcmp(op_array->vars[i].name, name, name_len)) {
			efree(name);
			return i;
		}
		i++;
	}
	i = op_array->last_var;
	op_array->last_var++;
	if (op_array->last_var > op_array->size_var) {
		op_array->size_var += 16; /* FIXME */
		op_array->vars = erealloc(op_array->vars, op_array->size_var*sizeof(zend_compiled_variable));
	}
	op_array->vars[i].name = name;
	op_array->vars[i].name_len = name_len;
	op_array->vars[i].hash_value = hash_value;
	return i;
}

static int basic_regitster_function(char *name, int name_len, zend_op *op)
{
	zend_op_array func;

	init_op_array(&func, ZEND_USER_FUNCTION, INITIAL_OP_ARRAY_SIZE TSRMLS_CC);
	func.function_name = estrndup(name, name_len);
	func.return_reference = 0;
	func.pass_rest_by_reference = 0;

	func.scope = NULL;
	func.prototype = NULL;

	func.line_start = 0;
	zend_op *opline = get_next_op(&func TSRMLS_CC);
	opline->opcode = ZEND_NOP;
	SET_UNUSED(opline->op1);
	SET_UNUSED(opline->op2);
/*
	opline = get_next_op(&func TSRMLS_CC);
	opline->opcode = ZEND_ASSIGN;
	opline->op1.op_type = IS_CV;
	opline->op1.u.var = lookup_cv(&func, "v", 1);
	opline->op2.op_type = IS_CONST;
	INIT_ZVAL(opline->op2.u.constant);
	ZVAL_STRING(&opline->op2.u.constant, "test", 1);
*/
	opline = get_next_op(&func TSRMLS_CC);
	opline->opcode = ZEND_ECHO;
	opline->op1.op_type = IS_CONST;
	INIT_ZVAL(opline->op1.u.constant);
	ZVAL_STRING(&opline->op1.u.constant, "test", 1);
	SET_UNUSED(opline->op2);

	opline = get_next_op(&func TSRMLS_CC);
	opline->opcode = ZEND_RETURN;
	opline->op1.op_type = IS_CONST;
	INIT_ZVAL(opline->op1.u.constant);
	ZVAL_STRING(&opline->op1.u.constant, "test", 1);
	SET_UNUSED(opline->op2);


	pass_two(&func TSRMLS_CC);

	zend_hash_update(EG(function_table), name, name_len+1, &func, sizeof(zend_op_array), NULL);

}

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

	basic_regitster_function(arg,arg_len,NULL);
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
const zend_function_entry basic_funcs[] = {
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
	basic_funcs,
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
