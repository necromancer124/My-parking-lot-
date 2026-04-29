## 🚗 Parking Lot Simulation (C)

A console-based parking lot simulator written in C that models real-time vehicle flow, payments, and enforcement. The system supports disabled parking logic, configurable pricing, fines, and persistent settings.

---

## ✨ Features

### 🅿️ Dynamic Parking Lot

* Configure the number of **rows** and **columns**
* Flexible lot size to fit different scenarios

### ♿ Disabled Parking Support

* Automatically assigns a percentage of spaces as disabled
* 25% chance that entering cars are disabled
* Disabled cars prioritize disabled spots

### 💰 Payment System

* Hourly parking rate (configurable)
* Automatic fee calculation on exit
* Fine system for improperly parked vehicles

### 🚓 Enforcement (Drag & Fine)

* Cars parked in the wrong spot are:

  * Dragged out
  * Fined according to configured amount

### ⏱ Real-Time Simulation

* Random car entry/exit events
* 1–4 second randomized delay between updates
* Non-blocking input — press any key to stop simulation

### 💾 Persistent Configuration

* Settings saved to `settings.txt`
* Automatically loaded at startup

### 🎨 Colored Console Output

* 🟢 Green → Free or disabled spot
* 🔴 Red → Occupied spot

### 📘 Built-in Help Menu

* Command list
* Usage explanation

---

## 🛠 Getting Started

### Prerequisites

* GCC (recommended) or any C compiler
* Windows, Linux, or macOS terminal

---

## 🔧 Build

```bash
gcc -o parkingLot parkingLot.c
```

---

## ▶️ Run

### Linux / macOS

```bash
./parkingLot
```

### Windows

```bash
parkingLot.exe
```

---

## 🖥 Available Commands

| Command    | Description                              |
| ---------- | ---------------------------------------- |
| `simulate` | Start simulation           |
| `settings` | Configure rows, columns, price, and fine |
| `reset`    | Delete saved settings file               |
| `help`     | Show help menu                           |
| `quit`     | Exit program                             |

---

## 🎮 During Simulation

* Cars randomly enter or exit
* Disabled cars use disabled spaces
* Payment is calculated on exit
* Improper parking results in drag + fine
* Press **any key** to stop simulation and return to menu

---

## ⚙️ Configuration File

Settings are stored in:

```
settings.txt
```

### Stored Parameters

| Parameter | Description            |
| --------- | ---------------------- |
| `ROWX`    | Number of rows         |
| `ROWY`    | Number of columns      |
| `PRICE`   | Parking price per hour |
| `FINE`    | Drag fine amount       |

---

## 🧠 Simulation Logic

* Randomized vehicle generation

* 25% probability of disabled vehicle

* Real-time hour tracking

* Automatic fee calculation:

  ```
  payment = hours_parked × PRICE
  ```

* Fine applied if parked incorrectly:

  ```
  total = payment + FINE
  ```

---

## 📂 Project Structure

```
parkingLot.c
settings.txt
README.md
```

---

## 🚀 Future Improvements (Optional Ideas)

* Vehicle ID tracking
* Revenue statistics summary
* Multi-level parking support
* File-based logging system
* Graphical interface (SDL or ncurses)

---

## 📜 License

This project is open-source and free to use.
