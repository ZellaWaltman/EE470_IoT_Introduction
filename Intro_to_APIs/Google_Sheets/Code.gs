// Sheet Credentials
//------------------------------------------------------------
const SHEET_ID   = '1OmxpLAGcMM1Go_5gkwbxE4U937lodDG4E9ruQWATWWo';
const STATE_SHEET = 'log'; // Sheet Name

// Sheet Read Functions
//------------------------------------------------------------
function getState() {
  // Return Latest State
  return getLatestState();
}

// Open sheet & ensure  target tab exists w/ headers
function _sh_() {
  const ss = SpreadsheetApp.openById(SHEET_ID);
  let sh = ss.getSheetByName(STATE_SHEET);
  if (!sh) {
    // Create sheet on 1st run & seed header row
    sh = ss.insertSheet(STATE_SHEET);
    sh.getRange('A1:E1').setValues([['Timestamp','LED State','R','G','B']]);
  }
  return sh;
}

// Add a new state row to sheet for current states
function appendState(led, r, g, b) {
  led = String(led||'OFF').toUpperCase();
  r = clamp255_(r); g = clamp255_(g); b = clamp255_(b);
  // Get client-side Timestamp (new Date())
  _sh_().appendRow([new Date(), led, r, g, b]);
  return getLatestState(); // return what was just set
}

// Read latest (last non-empty) row as current state
function getLatestState() {
  const sh = _sh_();
  let r = sh.getLastRow();
  if (r < 2) return { led: 'OFF', r: 0, g: 0, b: 0 };

  // read a chunk from the bottom up (avoid empty trailing rows)
  const start = Math.max(2, r - 200); // last 200 rows window
  const vals = sh.getRange(start, 1, r - start + 1, 5).getValues(); // A:E

  // Iterate from newest to oldest until a timestamped row is found
  for (let i = vals.length - 1; i >= 0; i--) {
    const [ts, led, rr, gg, bb] = vals[i];
    if (ts) {
      return {
        led: String(led || 'OFF'),
        r: Number(rr)||0, g: Number(gg)||0, b: Number(bb)||0
      };
    }
  }

  // Fallback in case nothing valid found in window
  return { led: 'OFF', r: 0, g: 0, b: 0 };
}

// Clamp ints to 0-255 for RGB Values
function clamp255_(v){ v=parseInt(v,10); if(isNaN(v)) v=0; return Math.max(0,Math.min(255,v)); } // Non-Number inputs = 0

// Web App Functions
//------------------------------------------------------------
//   GET /exec?fmt=led = "ON"/"OFF"
//   GET /exec?fmt=rgb = "R,G,B"
//   GET /exec = static page
function doGet(e) {
  const fmt = (e.parameter.fmt||'').toLowerCase();
  const st = getLatestState();
  if (fmt === 'led') {
    return ContentService.createTextOutput(st.led).setMimeType(ContentService.MimeType.TEXT);
  }
  if (fmt === 'rgb') {
    return ContentService.createTextOutput(`${st.r},${st.g},${st.b}`).setMimeType(ContentService.MimeType.TEXT);
  }
  // Minimal JSON if you visit /exec directly
  return ContentService.createTextOutput(JSON.stringify(st)).setMimeType(ContentService.MimeType.JSON);
}

// Allow writes through POST from device or web form
function doPost(e) {
  const p = e.parameter || {};
  if (p.led !== undefined || (p.r!==undefined && p.g!==undefined && p.b!==undefined)) {
    appendState(p.led ?? 'OFF', p.r ?? 0, p.g ?? 0, p.b ?? 0);
  }
  return ContentService.createTextOutput('OK').setMimeType(ContentService.MimeType.TEXT);
}
