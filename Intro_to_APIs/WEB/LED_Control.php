<?php

// Page for LED ON/OFF control and State display

// Allow cross-origin requests (for ESP or browser clients)
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, PUT, OPTIONS');
header('Access-Control-Allow-Headers: content-type');

// Path to state file results.txt
$resultsFile = __DIR__ . '/results.txt';

// Process PUT requests - ESP sends "ON"/"OFF":
//---------------------------------------------------
if ($_SERVER['REQUEST_METHOD'] === 'PUT') {
  $body = strtoupper(trim(file_get_contents('php://input'))); // Read request body
  // Only "ON" or "OFF" allowed
  if ($body === 'ON' || $body === 'OFF') { 
    file_put_contents($resultsFile, $body, LOCK_EX);
    header('Content-Type: text/plain; charset=utf-8');
    echo $body;
    exit;
  }
  // Invalid input = 400 Bad Request
  http_response_code(400);
  echo "Body must be ON or OFF";
  exit;
}

// Read current state for display 
//---------------------------------------------------
$state = 'OFF';
if (is_readable($resultsFile)) {
  $v = strtoupper(trim(file_get_contents($resultsFile)));
  if ($v === 'ON' || $v === 'OFF') $state = $v;
}
?>

<!doctype html>
<html lang="en">
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width,initial-scale=1">
        <title>ESP8266 LED Control</title>
        <style>
            body{font:14px system-ui,Arial;margin:3rem auto;max-width:480px}
            .card{border:1px solid #ddd;border-radius:12px;padding:1rem;box-shadow:0 1px 4px rgba(0,0,0,.06)}
            button{margin:.3rem .4rem;padding:.5rem 1rem;border-radius:8px;border:1px solid #bbb;cursor:pointer}
            .pill{padding:.2rem .6rem;border-radius:999px;background:#eee;font-weight:600}
            .on{background:#d3f9d8} .off{background:#ffd8d8}
        </style>
    </head>
    <body>
      <h2>ESP8266 LED Control</h2>
      <div class="card">
        <p>LED State:
          <span id="status" class="pill <?php echo strtolower($state);?>">
            <?php echo htmlspecialchars($state);?>
          </span>
        </p>
            <button onclick="setLED('ON')">Turn ON</button>
            <button onclick="setLED('OFF')">Turn OFF</button>
        <p><small>State stored in results.txt.</small></p>
      </div>
    
        <script>

            // JavaScript - button press functions
            // ------------------------------------------------------------
            async function setLED(val){
              try{
                const res = await fetch(location.href, {method:'PUT', body:val});
                const txt = (await res.text()).trim();
                document.getElementById('status').textContent = txt;
                document.getElementById('status').className = 'pill ' + txt.toLowerCase();
              }catch(e){alert('Request failed');}
            }
            function refresh(){ location.reload(); } /** refresh to display new state **/
        </script>
    </body>
</html>
