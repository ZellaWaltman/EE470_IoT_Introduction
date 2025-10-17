<?php
//setting header to json
header('Content-Type: application/json');

//database
define('DB_HOST', 'localhost');
define('DB_USERNAME', 'u551180265_db_ZellWaltman');
define('DB_PASSWORD', 'EE470myDB');
define('DB_NAME', 'u551180265_ZellaWaltman');

//get connection
$mysqli = new mysqli(DB_HOST, DB_USERNAME, DB_PASSWORD, DB_NAME);

if(!$mysqli){
  die("Connection failed: " . $mysqli->error);
}

//query to get data from the table
$query = sprintf("SELECT node_name, temperature, time_received FROM sensor_data WHERE node_name = 'node_1' ORDER BY time_received ASC");

//execute query
$result = $mysqli->query($query);

//loop through the returned data
$data = array();
foreach ($result as $row) {
  $data[] = $row;
}

//close connection
$mysqli->close();

//now print the data
print json_encode($data);
