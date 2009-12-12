--TEST--
Simple PRINT test
--SKIPIF--
<?php if (!extension_loaded("basic")) print "skip"; ?>
--FILE--
<?php 
basic_compile("t001", __DIR__.'/001.bas');
var_dump(t001());
?>
--EXPECT--
HALLO WELT
ENDE
NULL
