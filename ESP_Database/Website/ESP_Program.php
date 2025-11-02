<?php
    
    mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
    ini_set('display_errors', '1');
    ini_set('display_startup_errors', '1');
    error_reporting(E_ALL);

    $servername = "localhost"; // MySQL server's IP address/hostname
    $username = "u551180265_db_ESP_Program"; // MySQL username
    $password = "EE473espDB"; // MySQL password
    $dbname = "u551180265_ESP_Program"; // SQL DataBase Name
            
    // Create Connection w/ Database
    $conn = new mysqli($servername, $username, $password, $dbname);
            
    // Check Connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error); // Stop script & send error message
    }
    
    // If node=value in URL, use value.
    if (isset($_GET['action']) && $_GET['action'] === 'chart_data') {
        // Default to node_1 if not provided
        $node = $_GET['node'] ?? 'node_1';
    
        // fetch chart data, order by time (oldest to newest)
        $stmt = $conn->prepare("
            SELECT time_received, temperature
            FROM SensorData
            WHERE node_name = ?
            ORDER BY time_received ASC
        ");
        $stmt->bind_param("s", $node);
        $stmt->execute();
        $res = $stmt->get_result();
    
        // Formatting data as JSON
        $rows = [];
        // Loop through results -> php array
        while ($r = $res->fetch_assoc()) { $rows[] = $r; }
        $stmt->close();
        
        // Set to JSON & send to client
        header('Content-Type: application/json; charset=utf-8');
        echo json_encode($rows);
        $conn->close();
        exit; // IMPORTANT: stop here so no HTML is sent
    }
    
    // Get the average values and store them as json data
    if (isset($_GET['action']) && $_GET['action'] === 'stats') {
        $node = $_GET['node'] ?? 'node_1';
        $stmt = $conn->prepare("SELECT COUNT(*), AVG(temperature), AVG(humidity)
                                 FROM SensorData WHERE node_name = ?");
        $stmt->bind_param("s", $node);
        $stmt->execute();
        $stmt->bind_result($count, $avgT, $avgH);
        $stmt->fetch();
        $stmt->close();
    
        header('Content-Type: application/json; charset=utf-8');
        echo json_encode([
            'node' => $node,
            'count' => (int)$count,
            'avgTemp' => $avgT !== null ? round($avgT, 2) : null,
            'avgHum'  => $avgH !== null ? round($avgH, 2) : null
        ]);
        $conn->close();
        exit;
    }

    // Insert New Data w/ GET:
    //---------------------------------------
    
    $message = "";
    
    // All 3 parameters must have values. If so, convert to float.
    if (isset($_GET['node_name'], $_GET['time_received'], $_GET['temperature'], $_GET['humidity'])) {
        $node_name   = trim($_GET['node_name']);
        $time        = $_GET['time_received'] ?? ''; // ESP must provide time
        $temperature = (float)$_GET['temperature'];
        $humidity    = (float)$_GET['humidity'];
    
        // Range checks
        if ($temperature < -50 || $temperature > 150 || $humidity < 0 || $humidity > 100) {
            $message = "Error - Invalid values: temperature or humidity out of range.";
        } else {
            // Confirm node is registered - NO UNREGISTERED NODES ALLOWED
            $stmt = $conn->prepare("SELECT 1 FROM SensorRegister WHERE node_name = ?");
            $stmt->bind_param("s", $node_name);
            $stmt->execute();
            $stmt->store_result();
    
            if ($stmt->num_rows === 0) {
                $message = "Error - Node '$node_name' is not registered.";
            } else {
                // Require MCU-provided time; do NOT insert without it
                if ($time === '' || trim($time) === '') {
                    http_response_code(400);  // Bad Request
                    $message = "Error - time_received is required and must come from the device.";
                } else {
                    // Validate format "YYYY-MM-DD HH:MM:SS" or with microseconds
                    $dt = DateTime::createFromFormat('Y-m-d H:i:s.u', $time)
                        ?: DateTime::createFromFormat('Y-m-d H:i:s', $time);
                    if (!$dt) {
                        http_response_code(400);
                        $message = "Error - invalid time_received format.";
                    } else {
                        try {
                            $stmt2 = $conn->prepare(
                                "INSERT INTO SensorData (node_name, time_received, temperature, humidity)
                                 VALUES (?, ?, ?, ?)"
                            );
                            $stmt2->bind_param("ssdd", $node_name, $time, $temperature, $humidity);
                            $stmt2->execute();
                            $message = "Data inserted successfully for $node_name!";
                        } catch (mysqli_sql_exception $e) {
                            if ($e->getCode() == 1062) {
                                http_response_code(409); // Conflict
                                $message = "Error - Duplicate entry for node/time combination.";
                            } else {
                                http_response_code(500);
                                $message = "Error - Database error: " . htmlspecialchars($e->getMessage());
                            }
                        }
                    }
                }
            }
            
            $stmt->close();
            
        }
    }

            
    // Query 1: Get Sensor Register Table
    //---------------------------------------
    
    // Get SensorRegister data in Ascending Order (oldest to newest, a -> z)        
    $query1 = "SELECT * FROM SensorRegister ORDER BY node_name ASC";
     
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
    $query2 = "SELECT * FROM SensorData ORDER BY node_name ASC, time_received ASC";
        
    $result2 = $conn->query($query2);
        
    $Data = [];
    
    while ($row = $result2->fetch_assoc()) {
        $Data[] = $row;
    }
    
    // release data to take up less memory    
    $result2->free();
    
    // Query 3: Get Sensor Activity Table
    //---------------------------------------
    
    // Get sensor activity Node Name in Ascending Order (groups names together, A to Z)
    // AND then get count in Ascending Order (Oldest to Newest) 
    $query3 = "SELECT * FROM SensorActivity ORDER BY node_name ASC, data_count ASC";
        
    $result3 = $conn->query($query3);
        
    $Activity = [];
    
    while ($row = $result3->fetch_assoc()) {
        $Activity[] = $row;
    }
    
    // release data to take up less memory    
    $result3->free();
    
    // Get Average Value
    //---------------------------------------
    $stmt = $conn->prepare("SELECT COUNT(*), AVG(temperature), AVG(humidity)
                             FROM SensorData WHERE node_name = 'node_1'");
    $stmt->execute();
    $stmt->bind_result($avgCount, $avgTemp, $avgHum);
    $stmt->fetch();
    $stmt->close();

    $conn->close(); // Close Connection
        
    // Encode Arrays for JSON        
    $RegisterJS = json_encode($Register);
    $DataJS = json_encode($Data);
    $ActivityJS = json_encode($Activity);
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
        
        /* Node panel wraps dropdown + averages + chart */
        .node-panel {
            width: 95%;
            max-width: 1100px;
            margin: 20px auto 32px;
            background: #fff;
            border: 2px solid #999;
            border-radius: 10px;
            box-shadow: 0 2px 6px rgba(0,0,0,0.12);
            padding: 14px 16px 18px;
        }
        
        /* header with a dropdown menu */
        .panel-header {
            display: flex;
            align-items: center;
            gap: 10px;
            margin-bottom: 15px;
        }
        .panel-header label {
            font-weight: 600;
        }
        
        .panel-grid {
            display: flex;
            flex-direction: column; /* stack vertically */
            gap: 20px;              /* space between average and chart */
        }
        @media (max-width: 900px) {
          .panel-grid { grid-template-columns: 1fr; }
        }
        
        /* reuse average-box but remove outer margins so it fits panel */
        .node-panel .average-box { margin: 0; }
         
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
        
    </style>
        
</head>

<body>
    <h1>Sensor Information:</h1>
    
    <!-- Display message after GET insert -->
    <?php if ($message): ?>
        <div class="notice">
            <?php echo htmlspecialchars($message); ?>
        </div>
    <?php endif; ?>
    
    <!-- Average Display and Graph with Dropdown Menu -->
    <div class="node-panel">
        <div class="panel-header">
            <label for="nodeSelect">Select a Node to View:</label>
            <select id="nodeSelect"></select>
        </div>
        
        <div class="panel-grid">
            
            <!-- Average box (wrapped fields) -->
            <div class="average-box">
                <h2>Average for <span id="avgNode">node_1</span>:</h2>
                <p id="avgEmpty" style="display:none;">No data for this node yet.</p>
                <div id="avgVals">
                    <p>From <strong><span id="avgCount">0</span></strong> readings:</p>
                    <p>Average Temperature: <strong><span id="avgTemp">—</span> °C</strong></p>
                    <p>Average Humidity: <strong><span id="avgHum">—</span> %</strong></p>
                </div>
            </div>

            <!-- Chart -->
            <div class="chart-card" style="margin:0;">
                <?php $currentNode = $_GET['node'] ?? 'node_1'; ?>
                <iframe
                    id="chartFrame"
                    src="/ESP_Chart/ESP_graph.html?node=<?php echo htmlspecialchars($currentNode); ?>"
                    style="width:100%; height:440px; border:0; outline:none; display:block; margin:0 auto;"
                    scrolling="no"
                    loading="lazy">
                </iframe>
            </div>
        </div>
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
                        <th>Temperature (°C)</th>
                        <th>Humidity (%)</th>
                    </tr>
                </thead>
                    <tbody></tbody>
            </table>
        </div>
    
        <!-- Table 3 -->
        <div>
            <h2>Sensor Activity</h2>
            <table id="table3">
                <thead>
                    <tr>
                        <th>Node</th>
                        <th>Data Count</th>
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
        const data3 = <?php echo $ActivityJS ?: '[]'; ?>;
        
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
        
        // Fill Table 3 (Sensor Activity)
        fillTable(data3, '#table3 tbody', ['node_name','data_count']);
        
    </script>
    
    <script>
      // Read initial node from URL (?node=...), fallback to sessionStorage, then default
      const url = new URL(window.location.href);
      const urlNode = url.searchParams.get('node');
      const savedNode = sessionStorage.getItem('node');
      let currentNode = urlNode || savedNode || 'node_1';
    
      // Fill dropdown menu from SensorRegister
      (function initNodeDropdown() {
        const list  = <?php echo $RegisterJS ?: '[]'; ?>;
        const nodes = list.map(r => r.node_name);
        const sel   = document.getElementById('nodeSelect');
    
        // If Database = Empty:
        const options = (nodes.length ? nodes : ['node_1', 'node_2'])
          .map(n => `<option value="${n}">${n}</option>`).join("");
        sel.innerHTML = options;
    
        // If currentNode isn't in list, pick first available
        if (!nodes.includes(currentNode)) {
          currentNode = nodes[0] || 'node_1';
        }
        sel.value = currentNode; // <-- set dropdown to chosen node
      })();
    
      // Fetch & display averages for the node
      function renderAveragesFor(node) {
        fetch(`ESP_Program.php?action=stats&node=${encodeURIComponent(node)}`)
          .then(r => r.json())
          .then(s => {
            document.getElementById('avgNode').textContent = node;
            const empty = document.getElementById('avgEmpty');
            const vals  = document.getElementById('avgVals');
            if (!s || !s.count) {
              empty.style.display = 'block';
              vals.style.display  = 'none';
              return;
            }
            empty.style.display = 'none';
            vals.style.display  = 'block';
            document.getElementById('avgCount').textContent = s.count;
            document.getElementById('avgTemp').textContent  = (s.avgTemp ?? '—');
            document.getElementById('avgHum').textContent   = (s.avgHum  ?? '—');
          });
      }
    
      const sel    = document.getElementById('nodeSelect');
      const iframe = document.getElementById('chartFrame');
    
      function refreshPanel() {
        const node = sel.value || 'node_1';
        currentNode = node;
    
        // Update averages
        renderAveragesFor(node);
    
        // Update iframe (On user change or first load)
        iframe.src = `/ESP_Chart/ESP_graph.html?node=${encodeURIComponent(node)}`;
    
        // Persist selection in URL and sessionStorage so reload keeps it
        const u = new URL(window.location.href);
        u.searchParams.set('node', node);
        window.history.replaceState({}, '', u.toString());
        sessionStorage.setItem('node', node);
      }
    
      // On dropdown change, refresh panel and persist selection
      sel.addEventListener('change', refreshPanel);
    
      // Initial load with the resolved node
      (function init() {
        // Ensure the URL reflects current node on first paint
        const u = new URL(window.location.href);
        u.searchParams.set('node', currentNode);
        window.history.replaceState({}, '', u.toString());
        sessionStorage.setItem('node', currentNode);
    
        // Render averages and align iframe without forcing node_1
        sel.value = currentNode;
        renderAveragesFor(currentNode);

      })();
    </script>
    
</body>
</html>
