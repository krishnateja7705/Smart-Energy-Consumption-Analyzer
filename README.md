# Smart Energy Consumption Analyzer

A compact energy analysis project combining a C++ backend with a Node.js server and a web frontend.

## What it does

- Collects solar, battery, tariff, and device usage data
- Calculates monthly energy consumption and solar generation
- Estimates grid usage, monthly cost, and savings
- Identifies the top energy-consuming device
- Saves results to `output.json`
- Provides a frontend dashboard through `index.html`

## Project structure

- `backend.cpp` — C++ analyzer that reads input from stdin and writes `output.json`
- `server.js` — Node.js server that invokes the compiled backend and serves the web app
- `index.html` — frontend interface
- `input.txt` — example backend input
- `test_input.txt` — second example input
- `output.json` — generated analysis output

## Requirements

- C++ compiler (`g++`, `clang++`, or MSVC)
- Node.js installed
- Optional: web browser to open the frontend

## Build and run

### 1. Compile the C++ backend

On Windows with MinGW / g++:

```bash
g++ backend.cpp -o backend.exe
```

Or with MSVC developer tools:

```powershell
cl /EHsc backend.cpp /Fe:backend.exe
```

### 2. Start the Node.js server

```bash
node server.js
```

### 3. Open the frontend

Open a browser and go to:

```text
http://localhost:3000
```

## Input format

The backend expects data in the following order, each value on its own line:

1. Solar capacity in kW
2. Battery percent
3. Tariff in ₹/kWh
4. Location
5. Number of devices
6. For each device:
   - Device name
   - Room
   - Wattage
   - Hours per day

### Example (`input.txt`)

```text
5
75
8
India
3
AC
Living_Room
2000
6
Refrigerator
Kitchen
400
24
LED_Lights
Bedroom
100
5
```

## Output

The application writes `output.json` with:

- `location`
- `tariff_inr_per_kwh`
- `solar_capacity_kw`
- `battery_percent`
- `battery_hours_remaining`
- `total_consumption_kwh`
- `solar_generation_kwh`
- `grid_usage_kwh`
- `solar_coverage_pct`
- `monthly_bill_inr`
- `monthly_savings_inr`
- `top_device`
- `devices` list with per-device consumption and cost

## Notes

- `server.js` expects `backend.exe` to exist in the project root.
- If using a platform other than Windows, adjust the backend binary name accordingly.
- The frontend sends configuration data to `/api/analyze`, and the backend returns parsed JSON from `output.json`.

## License

Use and modify as needed.
