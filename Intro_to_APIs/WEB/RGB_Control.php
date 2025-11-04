<?php

// Path to rgb.php data — 3 sliders (0-255) saved like "R,G,B" in rgb.txt
$rgbFile = __DIR__ . '/rgb.txt';

// Default RGB values
$cur = [0,0,0];

// Read current RGB values from rgb.txt
// ------------------------------------------------------------
if (is_readable($rgbFile)) {
  // Read rgb.txt file contents
  $raw = trim(@file_get_contents($rgbFile));
  // Validate pattern "R,G,B"
  if (preg_match('/^\s*(\d{1,3})\s*,\s*(\d{1,3})\s*,\s*(\d{1,3})\s*$/', $raw, $m)) {
    // Clamp each value to max 255 and update
    $cur = [min(255,$m[1]), min(255,$m[2]), min(255,$m[3])];
  }
}

// Write new RGB values on Submit (POST)
// ---------------------------------------------------------------
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
  // Convert values to INTS and clamp 0–255
  $r = max(0, min(255, intval($_POST['r'] ?? $cur[0])));
  $g = max(0, min(255, intval($_POST['g'] ?? $cur[1])));
  $b = max(0, min(255, intval($_POST['b'] ?? $cur[2])));
  @file_put_contents($rgbFile, "{$r},{$g},{$b}\n", LOCK_EX);
  
  // Save RGB string to rgb.txt
  $cur = [$r,$g,$b];
}
?>

<!doctype html>
<html>
    <head>
    <meta charset="utf-8"/><meta name="viewport" content="width=device-width,initial-scale=1"/>
        <title>RGB Control (R,G,B)</title>
        <style>
            body{font:14px system-ui,Arial;max-width:640px;margin:2rem auto;padding:0 1rem}
            .card{border:1px solid #ddd;border-radius:12px;padding:1rem 1.25rem;box-shadow:0 1px 4px rgba(0,0,0,.06)}
            .row{margin:.8rem 0}
            label{display:flex;justify-content:space-between}
            .val{font-variant-numeric:tabular-nums;min-width:3ch;text-align:right}
            input[type=range]{width:100%}
        </style>
    </head><body>
      <h2>RGB Control</h2>
          <div class="card">
            <form method="post">
              <div class="row">
                <label>Red <span class="val" id="rv"><?php echo $cur[0];?></span></label>
                <input id="r" name="r" type="range" min="0" max="255" value="<?php echo $cur[0];?>"
                       oninput="rv.textContent=this.value">
              </div>
              <div class="row">
                <label>Green <span class="val" id="gv"><?php echo $cur[1];?></span></label>
                <input id="g" name="g" type="range" min="0" max="255" value="<?php echo $cur[1];?>"
                       oninput="gv.textContent=this.value">
              </div>
              <div class="row">
                <label>Blue <span class="val" id="bv"><?php echo $cur[2];?></span></label>
                <input id="b" name="b" type="range" min="0" max="255" value="<?php echo $cur[2];?>"
                       oninput="bv.textContent=this.value">
              </div>
              <button type="submit">Submit</button>
              <p>Current rgb.txt: <strong><?php echo implode(',', $cur);?></strong></p>
            </form>
          </div>
    </body>
</html>
