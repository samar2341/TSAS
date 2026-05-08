# Train Seat Allocation System

A terminal-based railway reservation and seat allocation system developed in C using structures, arrays, functions, file handling, and allocation logic.

This project simulates a small railway booking environment where users can:
- search trains
- book tickets
- cancel tickets
- view seat availability
- generate PNR numbers
- view booked tickets
- visualize seat allocation

The project is designed as a smart terminal-based utility with a clean minimal interface and practical railway-inspired features.

---

# Features

## Train Management
- View available trains
- Search trains by:
  - route
  - keyword
- Platform number display
- Live available seat count

---

## Ticket Booking
- Multiple passenger booking
- Automatic PNR generation
- Booking ID generation
- Duplicate booking prevention
- Window seat preference
- Coach allocation system
- Age-based fare calculation

---

## Smart Seat Allocation
The system uses custom seat allocation logic:

- Children (<12) get priority seats
- Senior citizens (>60) get early available seats
- Male passengers avoid sandwiched seats when possible
- Window seat preference supported

---

## Ticket System
Generated ticket includes:
- Booking ID
- PNR Number
- Passenger details
- Train details
- Coach and seat number
- Platform number
- Fare calculation
- Ticket status

---

## Seat Map
Interactive seat map display showing:
- Available seats
- Male occupied seats
- Female occupied seats
- Child occupied seats

---

## File Handling
All booking records are stored using:
```txt
tickets.txt
```

The project uses persistent file storage so bookings remain saved after program exit.

---

# Technologies Used

- C Programming
- Structures
- Arrays
- Functions
- File Handling
- ANSI Terminal Colors
- Linux Terminal

---

# Project Structure

```txt
main.c
tickets.txt
README.md
```

---

# Concepts Used

- Structures
- File Handling
- Arrays
- String Handling
- Conditional Logic
- Function Modularization
- Terminal UI
- Data Persistence

---

# Sample Features

## PNR Generation

Each ticket gets a unique PNR number:

```txt
PNR : 100001
```

---

## Fare Calculation

- Child (<12 years) → 50% discount
- Senior Citizen (>60 years) → 20% discount
- Others → Full fare

---

## Coach System

Seats are automatically mapped into coaches:

```txt
Seat 1-25  -> S1
Seat 26-50 -> S2
```

---

# How to Run

## Compile

```bash
gcc main.c -o railway
```

## Run

```bash
./railway
```

---

# Important Note

If upgrading from an older version of the project, delete old ticket records once:

```bash
rm tickets.txt
```

This is required because the newer version includes:
- PNR numbers
- updated file format

---

# Terminal UI

The project follows a minimal smart terminal style:
- clean colors
- compact interface
- loading indicators
- simple ASCII separators

The goal was to create a practical and readable command-line experience instead of an overly flashy UI.

---

# Future Improvements

- Waiting list system
- Admin panel
- Login system
- Dynamic train addition
- Database integration
- GUI version
- Online booking simulation

---

# Developed By

**Samar**

Graphic Era Hill University  
BCA Project / PBL

---

# License

This project is for educational and learning purposes.