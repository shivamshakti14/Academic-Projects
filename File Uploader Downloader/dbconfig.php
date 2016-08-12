<?php
$dbhost = "localhost";
$dbuser = "root";
$dbpass = "";
$dbname = "dbtuts";
$con=mysql_connect($dbhost,$dbuser,$dbpass) or die('cannot connect to the server'); 
$dblink=mysql_select_db($dbname) or die('database selection problem');
?>