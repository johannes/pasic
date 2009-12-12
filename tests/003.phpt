--TEST--
Simple END test
--SKIPIF--
<?php if (!extension_loaded("basic")) print "skip"; ?>
--FILE--
<?php 
include(__DIR__.'/003.bas');
?>
--EXPECT--
YES
