--TEST--
Test for database timeout option.
--FILE--
<?php
$m = new Mongo();
$d = $m->selectDb('admin');

try
{
	$d->command(array('sleep' => true, 'secs' => 2), array('timeout' => 1));
}
catch ( MongoCursorTimeoutException $e )
{
	var_dump( $e );
}
sleep( 2 );
?>
--EXPECTF--
object(MongoCursorTimeoutException)#%d (%d) {
  ["message":protected]=>
  string(56) "cursor timed out (timeout: 1, time left: 0:0, status: 0)"
  ["string":"Exception":private]=>
  string(0) ""
  ["code":protected]=>
  int(0)
  ["file":protected]=>
  string(%d) "%s"
  ["line":protected]=>
  int(7)
  ["trace":"Exception":private]=>
  array(1) {
    [0]=>
    array(6) {
      ["file"]=>
      string(%d) "%s"
      ["line"]=>
      int(7)
      ["function"]=>
      string(7) "command"
      ["class"]=>
      string(7) "MongoDB"
      ["type"]=>
      string(2) "->"
      ["args"]=>
      array(2) {
        [0]=>
        array(2) {
          ["sleep"]=>
          bool(true)
          ["secs"]=>
          int(2)
        }
        [1]=>
        array(1) {
          ["timeout"]=>
          int(1)
        }
      }
    }
  }
  ["previous":"Exception":private]=>
  NULL
  ["host":"MongoCursorException":private]=>
  NULL
  ["fd":"MongoCursorException":private]=>
  int(0)
}
