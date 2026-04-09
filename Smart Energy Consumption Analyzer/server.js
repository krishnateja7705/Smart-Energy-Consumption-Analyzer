const http = require('http');
const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');

const PORT = 3000;
const PROJECT_DIR = __dirname;

// Serve static files
function serveStaticFile(res, filePath) {
  fs.readFile(filePath, (err, data) => {
    if (err) {
      res.writeHead(404, { 'Content-Type': 'text/plain' });
      res.end('404 Not Found');
      return;
    }
    
    const ext = path.extname(filePath);
    let contentType = 'text/html';
    if (ext === '.json') contentType = 'application/json';
    else if (ext === '.js') contentType = 'application/javascript';
    else if (ext === '.css') contentType = 'text/css';
    
    res.writeHead(200, { 'Content-Type': contentType });
    res.end(data);
  });
}

// Run C++ backend with input
function runBackend(input, callback) {
  const backend = spawn(path.join(PROJECT_DIR, 'backend.exe'));
  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    // Read output.json if it was created
    const outputPath = path.join(PROJECT_DIR, 'output.json');
    fs.readFile(outputPath, 'utf8', (err, jsonData) => {
      if (err) {
        callback(null, 'Error reading output.json');
      } else {
        callback(JSON.parse(jsonData), null);
      }
    });
  });

  // Send input to backend
  backend.stdin.write(input);
  backend.stdin.end();
}

// Create server
const server = http.createServer((req, res) => {
  // CORS headers
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') {
    res.writeHead(200);
    res.end();
    return;
  }

  const url = req.url;

  // API: Run backend with configuration
  if (url === '/api/analyze' && req.method === 'POST') {
    let body = '';
    req.on('data', chunk => body += chunk);
    req.on('end', () => {
      try {
        const config = JSON.parse(body);
        
        // Build input string for C++ backend
        let input = `${config.solar}\n`;
        input += `${config.battery}\n`;
        input += `${config.tariff}\n`;
        input += `${config.location || 'India'}\n`;
        input += `${config.devices.length}\n`;
        
        for (const device of config.devices) {
          input += `${device.name}\n`;
          input += `${device.room}\n`;
          input += `${device.watt}\n`;
          input += `${device.hours}\n`;
        }

        runBackend(input, (data, err) => {
          if (err) {
            res.writeHead(500, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify({ error: err }));
          } else {
            res.writeHead(200, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify(data));
          }
        });
      } catch (e) {
        res.writeHead(400, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ error: e.message }));
      }
    });
    return;
  }

  // API: Get stored output.json
  if (url === '/api/output' && req.method === 'GET') {
    const outputPath = path.join(PROJECT_DIR, 'output.json');
    serveStaticFile(res, outputPath);
    return;
  }

  // Serve index.html for root
  if (url === '/' || url === 'index.html') {
    serveStaticFile(res, path.join(PROJECT_DIR, 'index.html'));
    return;
  }

  // Serve other static files
  const filePath = path.join(PROJECT_DIR, url);
  if (fs.existsSync(filePath)) {
    serveStaticFile(res, filePath);
    return;
  }

  res.writeHead(404, { 'Content-Type': 'text/plain' });
  res.end('404 Not Found');
});

server.listen(PORT, () => {
  console.log(`\n╔════════════════════════════════════════╗`);
  console.log(`║  Smart Energy Analyzer Server Started  ║`);
  console.log(`╚════════════════════════════════════════╝\n`);
  console.log(`  🌐 Open: http://localhost:${PORT}`);
  console.log(`  📊 Backend: Connected (C++)`);
  console.log(`  ✓ Ready to analyze energy systems\n`);
});
