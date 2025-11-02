<?php
//setting header to json
header('Content-Type: application/json; charset=utf-8');

//database
define('DB_HOST', 'localhost');
define('DB_USERNAME', 'u551180265_db_ESP_Program');
define('DB_PASSWORD', 'EE473espDB');
define('DB_NAME', 'u551180265_ESP_Program');

//get connection
$mysqli = new mysqli(DB_HOST, DB_USERNAME, DB_PASSWORD, DB_NAME);

$mysqli->set_charset('utf8mb4');

if(!$mysqli){
  die("Connection failed: " . $mysqli->error);
}

// query to get data from the table
//-----------------------------------------------

// get node from query (default node_1)
$node = isset($_GET['node']) && $_GET['node'] !== '' ? $_GET['node'] : 'node_1';

$stmt = $mysqli->prepare("
  SELECT node_name, temperature, humidity, time_received
  FROM SensorData
  WHERE node_name = ?
  ORDER BY time_received ASC
");

// execute query
$stmt->bind_param('s', $node);
$stmt->execute();
$result = $stmt->get_result();

// Close Statement
$stmt->close();

// loop through the returned data
$data = array();
foreach ($result as $row) {
  $data[] = $row;
}

//close connection
$mysqli->close();

//now print the data
print json_encode($data);
