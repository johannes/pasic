--TEST--
Simple END test
--SKIPIF--
<?php if (!extension_loaded("pasic")) print "skip"; ?>
--FILE--
<?php 
include(__DIR__.'/003.bas');
?>
--EXPECT--
YES
