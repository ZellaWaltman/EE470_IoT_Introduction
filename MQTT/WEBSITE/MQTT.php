<?php
// pot_viewer.php

// Show all errors while you're debugging
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
ini_set('display_errors', '1');
ini_set('display_startup_errors', '1');
error_reporting(E_ALL);

// DB CONNECTION
//--------------------------------------------------------------------------------------------
$servername = "localhost"; // MySQL server's IP address/hostname
$username = "u551180265_db_ESP_Program"; // MySQL username
$password = "EE473espDB"; // MySQL password
$dbname = "u551180265_ESP_Program"; // SQL DataBase Name

// Create Connection w/ Database
$conn = new mysqli($servername, $username, $password, $dbname);

// Check Connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// If node=value in URL, use value.
if (isset($_GET['action']) && $_GET['action'] === 'chart_data') {
    // fetch chart data, order by time (oldest to newest)
    $stmt = $conn->prepare("
        SELECT time_received, potentiometer_value
        FROM potentiometer_values
        ORDER BY time_received ASC
    ");
    $stmt->execute();
    $res = $stmt->get_result();
    
    // Formatting data as JSON
    $rows = [];
    
    // Loop through results -> php array
    while ($r = $res->fetch_assoc()) {
        $rows[] = $r;
    }
    $stmt->close();
    
    // Set to JSON & send to client
    header('Content-Type: application/json; charset=utf-8');
    echo json_encode($rows);
    $conn->close();
    exit; // IMPORTANT: stop here so no HTML is sent
}

// MAIN PAGE: Table data
$query = "
    SELECT id, time_received, potentiometer_value
    FROM potentiometer_values
    ORDER BY time_received DESC
";

$result = $conn->query($query);

$PotData = [];
while ($row = $result->fetch_assoc()) {
    $PotData[] = $row;
}
$result->free();
$conn->close();

// Encode for JS
$PotDataJS = json_encode($PotData);
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8" />
    <title>MQTT Potentiometer Data</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #fafafa;
        }
        h1 {
            border: 2px solid #999;
            padding: 10px 20px;
            background-color: #f2f2f2;
            border-radius: 6px;
            text-align: center;
            width: 90%;
            margin: 20px auto;
        }
        .chart-card {
            width: 95%;
            max-width: 900px;
            margin: 20px auto;
            background: #fff;
            border: 2px solid #999;
            border-radius: 10px;
            box-shadow: 0 2px 6px rgba(0,0,0,0.12);
            padding: 16px 18px;
        }
        .chart-card h2 {
            margin-top: 0;
            text-align: center;
        }
        table {
            border-collapse: collapse;
            width: 95%;
            max-width: 900px;
            margin: 20px auto;
            background-color: #fff;
            box-shadow: 0 1px 4px rgba(0,0,0,0.1);
        }
        thead th {
            border: 1px solid #000000;
            background-color: #e6e6e6;
            color: #000;
            font-weight: bold;
            padding: 8px;
        }
        td {
            border: 1px solid #000000;
            padding: 6px;
            text-align: center;
        }
        tbody tr:nth-child(even) { background-color: #f9f9f9; }
        tbody tr:hover { background-color: #f1f1f1; }
    </style>
</head>
<body>
    <h1>MQTT Potentiometer Voltage Readings</h1>

    <!-- Chart (Graph) -->
<div class="chart-card">
    <h2>Potentiometer Voltage Graph</h2>
    <iframe
        src="https://zellawaltman.org/MQTT_Chart/MQTT_graph.html"
        style="width:100%; height:440px; border:0; outline:none; display:block; margin:0 auto;"
        scrolling="no"
        loading="lazy">
    </iframe>
</div>

    <!-- Potentiometer Value Table  -->
    <div>
        <h2 style="text-align:center;">Potentiometer Values</h2>
        <table id="potTable">
            <thead>
                <tr>
                    <th>ID</th>
                    <th>Time Received</th>
                    <th>Potentiometer Value (Voltage)</th>
                </tr>
            </thead>
            <tbody></tbody>
        </table>
    </div>

    <script>
        // Data embedded from PHP for the table
        const potData = <?php echo $PotDataJS ?: '[]'; ?>;

        // Fill the HTML table
        const tbody = document.querySelector('#potTable tbody');
        tbody.innerHTML = '';
        for (const row of potData) {
            const tr = document.createElement('tr');

            const tdId   = document.createElement('td');
            const tdTime = document.createElement('td');
            const tdVal  = document.createElement('td');

            tdId.textContent   = row.id;
            tdTime.textContent = row.time_received;
            tdVal.textContent  = row.potentiometer_value;

            tr.appendChild(tdId);
            tr.appendChild(tdTime);
            tr.appendChild(tdVal);

            tbody.appendChild(tr);
        }
        
        // Optional: refresh every 10 seconds
        // setInterval(loadChartData, 10000);
        
    </script>
</body>
</html>
