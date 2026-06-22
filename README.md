# 🚗 Parking Lot Simulation
A cross-platform Terminal User Interface (TUI) simulation of a smart parking lot system, featuring dynamic car entry/exit, disabled spot management, and revenue tracking.

## 🚀 Overview
This project simulates the daily operations of a parking lot. It uses a grid-based system to track available spaces, manage priority parking for disabled vehicles, and calculate profit based on hourly rates and fines for illegally dragged cars.

## ✨ Features
- **Dynamic Simulation**: Cars enter and exit based on random probability, simulating real-world traffic.
- **Priority Parking**: Dedicated spots for disabled vehicles with strict enforcement.
- **Financial Tracking**: Calculates total profit based on parking duration and fines.
- **TUI Experience**: A clean, interactive terminal menu with keyboard navigation and color-coded status grids.
- **Persistence**: Saves your parking lot configuration (rows, columns, prices) to a local file.
- **Audit Log**: Maintains a `carsHistory.txt` log of every payment and event.

## 🛠️ Build & Run

### Prerequisites
- GCC Compiler
- Make (for Linux/macOS)

### Building (Linux/macOS/Windows with MinGW)
```bash
make
```

### Running
```bash
./parking_lot
```

## 🕹️ How it Works
- **Green Spots**: Free or Disabled spots.
- **Red Spots**: Occupied spaces.
- **Simulation**: Press "Simulate" to watch the lot fill and empty in real-time.
- **Management**: Use the settings menu to customize the size and pricing of your lot.

## 📜 License
This project is licensed under the MIT License.
