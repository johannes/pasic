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

static zend_uint get_temporary_variable(zend_op_array *op_array) 
{
	return (op_array->T)++ * sizeof(temp_variable);
}

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

static void basic_init_op_array(zend_op_array *op)
{
	init_op_array(op, ZEND_USER_FUNCTION, INITIAL_OP_ARRAY_SIZE TSRMLS_CC);
	op->return_reference = 0;
	op->pass_rest_by_reference = 0;

	op->scope = NULL;
	op->prototype = NULL;

	op->line_start = 1;
}

static int basic_compile_line(zend_op_array *op, char *line)
{
	zend_op *opline;
	char *token;
	int lineno = atoi(line);

	if (!lineno) {
		if (line[0] == 'R' && line[1] == 'E' && line[2] == 'M') {
			return SUCCESS;
		} else {
			return FAILURE;
		}
	}
	
	if (lineno <= op->last) {
		php_error_docref(NULL, E_WARNING TSRMLS_CC, "Can not redeclare line %d, expecting line >%d", lineno, op->last);
		return FAILURE;
	}

	while ((opline = get_next_op(op TSRMLS_CC)) && op->last < lineno) {
		opline->opcode = ZEND_NOP;
	}

	token = strtok(line, " \t");
	token = strtok(NULL, " \t");

	if (!token) {
		return SUCCESS;
	}

	if (memcmp(token, "PRINT", sizeof("PRINT")) == 0) {
		int token_len;
		token = strtok(NULL, "\"");
		if (!token) {
			return FAILURE;
		}

		opline->opcode = ZEND_ECHO;
		opline->op1.op_type = IS_CONST;
		ZVAL_STRING(&opline->op1.u.constant, token, 1);
		SET_UNUSED(opline->op2);
	}



	return SUCCESS;
}

static int basic_compile_file(zend_op_array *op, char *file, int file_len)
{
	zend_op *opline;	

	php_stream *stream = php_stream_open_wrapper(file, "rb", REPORT_ERRORS, NULL);
	if (!stream) {
		return FAILURE;
	}

	while(!php_stream_eof(stream)) {
		char line[1024];
		if (php_stream_gets(stream, line, sizeof(line))) {
			if (basic_compile_line(op, line) == FAILURE) {
				php_stream_close(stream);
				return FAILURE;
			}
		} else {
			break;
		}
	}
	php_stream_close(stream);

	opline = get_next_op(op TSRMLS_CC);
	opline->opcode = ZEND_RETURN;
	opline->op1.op_type = IS_CONST;
	INIT_ZVAL(opline->op1.u.constant);
	SET_UNUSED(opline->op2);

	return SUCCESS;
}

static void basic_regitster_function(char *name, int name_len, zend_op_array *op)
{
	op->function_name = estrndup(name, name_len);
	pass_two(op TSRMLS_CC);
	zend_hash_update(EG(function_table), name, name_len+1, op, sizeof(zend_op_array), NULL);

}

/* {{{ proto string basic_compile(string function_name, string filename)
   compile filename into function function_name */
PHP_FUNCTION(basic_compile)
{
	zend_op_array op;
	char *name, *file;
	int name_len, file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &file, &file_len) == FAILURE) {
		return;
	}

	basic_init_op_array(&op);
	if (basic_compile_file(&op, file, file_len) == FAILURE) {
		RETURN_FALSE;
	}
	basic_regitster_function(name, name_len, &op);
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
