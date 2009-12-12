--TEST--
Simple line number test
--SKIPIF--
<?php if (!extension_loaded("basic")) print "skip"; ?>
--FILE--
<?php 
include(__DIR__.'/002.bas');
?>
--EXPECTF--
Warning: include(): Can not redeclare line 10, expecting line >20 in %s on line %d

