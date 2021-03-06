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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_pasic.h"

static zend_op_array *(*orig_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);

static void pasic_init_op_array(zend_op_array *op) /* {{{ */
{
	init_op_array(op, ZEND_USER_FUNCTION, INITIAL_OP_ARRAY_SIZE TSRMLS_CC);
	op->return_reference = 0;
	op->pass_rest_by_reference = 0;

	op->scope = NULL;
	op->prototype = NULL;

	op->line_start = 1;
}
/* }}} */

static uint find_opline_for_lineno(zend_op_array *op, uint lineno) /* {{{ */
{
	uint i;
	for (i = 0; i < op->last; i++) {
		if (op->opcodes[i].lineno >= lineno) {
			return i;
		}
	}
	return 0;
}
/* }}} */

static int pasic_compile_line(zend_op_array *op, char *line) /* {{{ */
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

	opline = get_next_op(op TSRMLS_CC);
	opline->lineno = lineno;
	opline->opcode = ZEND_NOP;

	token = strtok(line, " \t\n\r");
	token = strtok(NULL, " \t\n\r");

	if (!token) {
		return SUCCESS;
	}

	if (memcmp(token, "PRINT", sizeof("PRINT")) == 0) {
		token = strtok(NULL, "\"");
		if (!token) {
			return FAILURE;
		}

		opline->opcode = ZEND_ECHO;
		opline->op1.op_type = IS_CONST;
		ZVAL_STRING(&opline->op1.u.constant, token, 1);
		SET_UNUSED(opline->op2);
	} else if (memcmp(token, "GOTO", sizeof("GOTO")) == 0) {
		int target;
		token = strtok(NULL, " \t\n");
		if (!token) {
			return FAILURE;
		}

		target = atoi(token);
		if (!target) {
			return FAILURE;
		}

		if (target != lineno) {
			opline->opcode = ZEND_JMP;
			opline->extended_value = target;
		} else {
			return FAILURE;
		}
	} else if (memcmp(token, "END", sizeof("END")) == 0) {
		opline->opcode = ZEND_RETURN;
		opline->op1.op_type = IS_CONST;
		INIT_ZVAL(opline->op1.u.constant);
		SET_UNUSED(opline->op2);
	} else {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

static int fix_jmps(zend_op_array *op) /* {{{ */
{
	int i;
	for (i = 0; i < op->last; i++) {
		if (op->opcodes[i].opcode == ZEND_JMP) {
			zend_op *opline = &op->opcodes[i];
			opline->op1.u.opline_num = find_opline_for_lineno(op, opline->extended_value);
			if (!opline->op1.u.jmp_addr) {
				return FAILURE;
			}
		}
	}
	return SUCCESS;
}
/* }}} */

static zend_op_array *pasic_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC) /* {{{ */
{
	zend_op_array *op;
	zend_op *opline;
	size_t filename_len = strlen(file_handle->filename);

	if (filename_len > 4 && memcmp(&file_handle->filename[filename_len-4], ".bas", 4) != 0) {
		return orig_compile_file(file_handle, type TSRMLS_CC);
	}

	op = emalloc(sizeof(zend_op_array));
	pasic_init_op_array(op);

	php_stream *stream = php_stream_open_wrapper(file_handle->filename, "rb", REPORT_ERRORS, NULL);
	if (!stream) {
		return NULL;
	}

	while(!php_stream_eof(stream)) {
		char line[1024];
		if (php_stream_gets(stream, line, sizeof(line))) {
			if (pasic_compile_line(op, line) == FAILURE) {
				php_stream_close(stream);
				return NULL;
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

	fix_jmps(op);

	pass_two(op TSRMLS_CC);
	return op;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pasic)
{
	orig_compile_file = zend_compile_file;
	zend_compile_file = pasic_compile_file;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pasic)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PASIC support", "enabled");
	php_info_print_table_end();
}   
/* }}} */

/* {{{ pasic_module_entry
 */
zend_module_entry pasic_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"pasic",
	NULL,
	PHP_MINIT(pasic),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(pasic),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1",
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PASIC
ZEND_GET_MODULE(pasic)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
