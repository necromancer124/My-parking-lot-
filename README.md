Parking Lot Simulation

A console-based parking lot simulator written in C. This project allows you to simulate cars entering and exiting a parking lot, including special handling for disabled spaces. The simulation tracks parking payments, fines for improperly parked cars, and supports customizable settings.

Features

Dynamic Parking Lot: Set the number of rows and columns.

Disabled Spaces: Automatically assigns a percentage of spots as disabled.

Car Simulation: Cars randomly enter and exit the lot. Disabled cars use disabled spots.

Payment System: Tracks parking fees per hour.

Drag Fines: Cars left in the wrong spot are dragged and fined.

Real-Time Simulation: Simulation updates in real time with random delays.

Non-Blocking Stop: Press any key to stop the simulation and return to the menu.

Persistent Settings: Save and load parking lot configuration, price, and fine.

Colored Console Display: Green = free/disabled, Red = occupied.

Help Menu: Instructions for using the program.

Getting Started
Prerequisites

C compiler (GCC recommended)

Windows, Linux, or macOS terminal

Build
gcc -o parkingLot parckingLot.c

Run
./parkingLot       # Linux/macOS
parkingLot.exe     # Windows

Usage

When you run the program, you can use the following commands:

Command	Description
simulate	Start the parking lot simulation from 6:00 to 22:00
settings	Change rows, columns, parking price, and drag fine
reset	Delete saved settings file
help	Show help menu
quit	Exit the program

During the simulation:

Cars randomly enter or exit.

Press any key to stop the simulation and return to the menu.

Configuration

Settings are stored in settings.txt and include:

Number of rows (ROWX)

Number of columns (ROWY)

Parking price per hour (PRICE)

Fine amount for dragged cars (FINE)

These settings are loaded automatically when the program starts.

Simulation Behavior

Green spots: Free or disabled

Red spots: Occupied

Cars enter randomly with a 25% chance of being disabled.

Exit payments are calculated based on the current hour and the hourly price.

Cars left in the wrong spot are dragged and fined.

Random delay of 1–4 seconds between updates to make the simulation dynamic.

License

This project is open-source and free to use.
