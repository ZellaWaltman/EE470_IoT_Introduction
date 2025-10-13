<?php
    
    $servername = "localhost"; // MySQL server's IP address/hostname
    $username = "u551180265_db_ZellWaltman"; // MySQL username
    $password = "EE470myDB"; // MySQL password
    $dbname = "u551180265_ZellaWaltman"; // SQL DataBase Name
            
    // Create Connection w/ Database
    $conn = new mysqli($servername, $username, $password, $dbname);
            
    // Check Connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error); // Stop script & send error message
    }
    
    if (isset($_GET['action']) && $_GET['action'] === 'chart_data') {
        // which node to plot (default node_1)
        $node = $_GET['node'] ?? 'node_1';
    
        // fetch only what's needed for the chart
        $stmt = $conn->prepare("
            SELECT time_received, temperature
            FROM sensor_data
            WHERE node_name = ?
            ORDER BY time_received ASC
        ");
        $stmt->bind_param("s", $node);
        $stmt->execute();
        $res = $stmt->get_result();
    
        $rows = [];
        while ($r = $res->fetch_assoc()) { $rows[] = $r; }
        $stmt->close();
    
        header('Content-Type: application/json; charset=utf-8');
        echo json_encode($rows);
        $conn->close();
        exit; // IMPORTANT: stop here so no HTML is sent
    }
    
    // Insert New Data w/ GET:
    //---------------------------------------
    
    $message = "";
    
    if (isset($_GET['node_name'], $_GET['temperature'], $_GET['humidity'])) {
        $node_name   = trim($_GET['node_name']);
        $temperature = (float)$_GET['temperature'];
        $humidity    = (float)$_GET['humidity'];
        $time        = $_GET['time_received'] ?? null;
    
        // --- Range checks (adjust to your assignment specs) ---
        if ($temperature < -50 || $temperature > 150 || $humidity < 0 || $humidity > 100) {
            $message = "Error - Invalid values: temperature or humidity out of range.";
        } else {
            // --- Confirm node is registered ---
            $stmt = $conn->prepare("SELECT 1 FROM sensor_register WHERE node_name = ?");
            $stmt->bind_param("s", $node_name);
            $stmt->execute();
            $stmt->store_result();
    
            if ($stmt->num_rows === 0) {
                $message = "Error - Node '$node_name' is not registered.";
            } else {
                // --- Insert new record ---
                try {
                    if ($time && $time !== "") {
                        $stmt2 = $conn->prepare("INSERT INTO sensor_data (node_name, time_received, temperature, humidity)
                                                  VALUES (?, ?, ?, ?)");
                        $stmt2->bind_param("ssdd", $node_name, $time, $temperature, $humidity);
                    } else {
                        // Let DB assign CURRENT_TIMESTAMP
                        $stmt2 = $conn->prepare("INSERT INTO sensor_data (node_name, temperature, humidity)
                                                  VALUES (?, ?, ?)");
                        $stmt2->bind_param("sdd", $node_name, $temperature, $humidity);
                    }
                    $stmt2->execute();
                    $message = "Data inserted successfully for $node_name!";
                } catch (mysqli_sql_exception $e) {
                    if ($e->getCode() == 1062) {
                        $message = "Error - Duplicate entry for node/time combination.";
                    } else {
                        $message = "Error - Database error: " . htmlspecialchars($e->getMessage());
                    }
                }
            }
            
            $stmt->close();
            
        }
    }
            
    // Query 1: Get Sensor Register Table
    //---------------------------------------
    
    // Get sensor_register data in Ascending Order (oldest to newest, a -> z)        
    $query1 = "SELECT * FROM sensor_register ORDER BY node_name ASC";
     
    // Run MySQL command and store result
    $result1 = $conn->query($query1); 
        
    $Register = [];
    
    // fetch one row at a time as a php associative array 
    while ($row = $result1->fetch_assoc()) {
        $Register[] = $row; // Add each row to Register array
    }
    
    // release data to take up less memory    
    $result1->free(); 
        
    // Query 2: Get Sensor Data Table
    //---------------------------------------
    
    // Get sensor data Node Name in Ascending Order (groups names together, A to Z)
    // AND then get sensor data Node Name in Ascending Order (Oldest to Newest) 
    $query2 = "SELECT * FROM sensor_data ORDER BY node_name ASC, time_received ASC";
        
    $result2 = $conn->query($query2);
        
    $Data = [];
    
    while ($row = $result2->fetch_assoc()) {
        $Data[] = $row;
    }
    
    // release data to take up less memory    
    $result2->free();
    
    // Get Average Value
    //---------------------------------------
    $stmt = $conn->prepare("SELECT COUNT(*), AVG(temperature), AVG(humidity)
                             FROM sensor_data WHERE node_name = 'node_1'");
    $stmt->execute();
    $stmt->bind_result($avgCount, $avgTemp, $avgHum);
    $stmt->fetch();
    $stmt->close();

    $conn->close(); // Close Connection
        
    // Encode Arrays for JSON        
    $RegisterJS = json_encode($Register);
    $DataJS = json_encode($Data);
?>

<!DOCTYPE HTML>

<html lang="en">
<head>
        
    <meta charset="utf-8" />
    <title>Zella's Sensor Data</title>
            
    <style>
    
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #fafafa;
        }
                      
        h1 {
            border: 2px solid #999; /* Gray outline */
            padding: 10px 20px; /* Space inside the box */
            background-color: #f2f2f2; /* Light gray background */
            border-radius: 6px; /* Round corners */
            text-align: center; /* Center text */
            width: 90%;                    
            margin: 20px auto; /* Center on page */
        }
                    
        h2 {
            text-align: left; /* Left on page */
            color: #333;
        }
        
        /* Creating a Box for the Average (just for visuals) */
        .average-box {
            max-width: 400px;
            margin: 20px;
            background-color: #ffffff;
            border: 2px solid #999;
            border-radius: 10px;
            box-shadow: 0 2px 6px rgba(0,0,0,0.15);
            padding: 15px 20px;
            text-align: left;
        }
        
        .average-box h2 {
            font-size: 1.1rem;   /* smaller text size (20px roughly) */
            font-weight: 600;     /* slightly bold */
            color: #333;
            margin-top: 0;
            margin-bottom: 10px;
        }
         
        /* Container - Will help with Table Formatting! */          
        .container {
            display: block;
        }
                    
        .container .col {
            width: 100%;
            margin-bottom: 30px;
        }
        
         /* TABLE LAYOUT */            
        table {
            border-collapse: collapse;
            width: 100%;
            margin-top: 10px;
            background-color: white;
            box-shadow: 0 1px 4px rgba(0, 0, 0, 0.1);
        }
                    
        /* Header row style */
        thead th {
            border: 1px solid #000000;
            background-color: #e6e6e6; /* light gray */
            color: #000;
            font-weight: bold;
            padding: 8px;
        }
                    
        /* Cell style */
        td {
            border: 1px solid #000000;
            padding: 6px;
            text-align: center;
        }
                    
        /* Zebra striping (for readability) */
        tbody tr:nth-child(even) {
            background-color: #f9f9f9;
        }
                    
        tbody tr:hover {
            background-color: #f1f1f1;
        }
        
         /* Graph Style */
        .chart-card {
            width: 100%;
            max-width: 900px;
            margin: 10px auto 24px;
            text-align: center;
            margin-bottom: 40px;
        }
        .chart-card iframe {
            display: block;
            margin: 0 auto;             /* center */
            border: none;               /* no grey border */
            border-radius: 8px;
            box-shadow: 0 2px 8px rgba(0,0,0,0.08);
        }
        #mycanvas {
            width: 10%;
            height: 10%;         
            display: block;
            border: 0;
            outline: 0;
        }
        
    </style>
        
</head>

<body>
    <h1>Sensor Information:</h1>
    
    <!-- Display message after GET insert -->
    <?php if ($message): ?>
        <div class="notice <?php echo str_starts_with($message, 'successfully') ? 'ok' : 'err'; ?>">
            <?php echo htmlspecialchars($message); ?>
        </div>
    <?php endif; ?>
    
    <!-- Average display -->
    <div class="average-box">
        <h2>Average for Node 1:</h2>
        
        <?php if ($avgCount > 0): ?>
            <p>From <?php echo $avgCount; ?> readings:</p>
            <p>Average Temperature: <strong><?php echo number_format($avgTemp, 2); ?> °C</strong></p>
            <p>Average Humidity: <strong><?php echo number_format($avgHum, 2); ?> %</strong></p>
            <?php else: ?>
            <p>No data for Node 1 yet.</p>
        <?php endif; ?>
        
    </div>
    
    <!-- Temperature Chart for Node 1 -->
    <div class="chart-card">
        <canvas id="mycanvas"></canvas>
        <iframe
            src="Chartjs/graph.html"
            width="700"
            height="360"
            loading="lazy">
      </iframe>
    </div>
        
        
    <div class="container">
        
        <!-- Table 1 -->
        <div>
            <h2>Registered Sensor Nodes</h2>
            <table id="table1">
                <thead>
                    <tr>
                        <th>Node</th>
                        <th>Manufacturer</th>
                        <th>Longitude</th>
                        <th>Latitude</th>
                    </tr>
                </thead>
                    <tbody></tbody>
            </table>
        </div>
        
        <!-- Table 2 -->
        <div>
            <h2>Data Received</h2>
            <table id="table2">
                <thead>
                    <tr>
                        <th>Node</th>
                        <th>Time Received</th>
                        <th>Temperature</th>
                        <th>Humidity</th>
                    </tr>
                </thead>
                    <tbody></tbody>
            </table>
        </div>
    </div>
    
    <!-- JavaScript -->
    <script>
        
        // Get JSON Arrays for Table Data. If empty, use empty array
        const data1 = <?php echo $RegisterJS ?: '[]'; ?>;
        const data2 = <?php echo $DataJS ?: '[]'; ?>;
        
        // Fill Tables with Data
        function fillTable(data, tbodySelector, cols) {
            
            // 
            const tbody = document.querySelector(tbodySelector);
            tbody.innerHTML = '';
            
            // For every row in JSON data
            for (const row of data) {
                
                const tr = document.createElement('tr'); // create a new table row
                
                // for every Column in JSON data
                for (const c of cols) {
                   
                    const td = document.createElement('td'); // create a new table cell
                    td.textContent = row[c] ?? ''; // Get column key and place data inside column
                    tr.appendChild(td); // Add data cell to row
                    
                }
                
            tbody.appendChild(tr); // Add complete row to table
            
            }
            
        }
        
        // Fill Table 1 (Sensor Register)
        fillTable(data1, '#table1 tbody', ['node_name','manufacturer','longitude','latitude']);
        
        // Fill Table 2 (Sensor Data)
        fillTable(data2, '#table2 tbody', ['node_name','time_received','temperature','humidity']);
        
    </script>
    
</body>
</html>
