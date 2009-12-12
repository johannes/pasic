--TEST--
Simple GOTO test
--SKIPIF--
<?php if (!extension_loaded("pasic")) print "skip"; ?>
--FILE--
<?php 
include(__DIR__.'/004.bas');
?>
--EXPECT--
AB
