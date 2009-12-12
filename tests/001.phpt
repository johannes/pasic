--TEST--
Simple PRINT test
--SKIPIF--
<?php if (!extension_loaded("basic")) print "skip"; ?>
--FILE--
<?php 
include(__DIR__.'/001.bas');
?>
--EXPECT--
HALLO WELT
