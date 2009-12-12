--TEST--
Simple PRINT test
--SKIPIF--
<?php if (!extension_loaded("basic")) print "skip"; ?>
--FILE--
<?php 
basic_compile("t001", __DIR__.'/001.bas');
$retval = t001();
echo "\n";
var_dump($retval);
?>
--EXPECT--
HALLO WELT
NULL
