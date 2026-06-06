# Robust Vending Machine FSM (Embedded C)

## Project Overview
This project implements a Finite State Machine (FSM) in Embedded C for an STM32 microcontroller. It simulates the control logic of an automatic vending machine across five distinct states: IDLE, SELECTION, PAYMENT, DISTRIBUTION, and ERROR.

## Architecture and Problem Solving
Traditional state machines in embedded systems often rely on deeply nested `switch-case` statements. While functional initially, this approach inherently generates high cyclomatic complexity. As the system grows, the code becomes difficult to maintain, scale, and thoroughly test.

To resolve this, the project refactors the FSM into a **Transition Table using Function Pointers**. By decoupling the machine's behavioral rules from its execution engine, we achieve significant structural improvements:
* **Reduced Complexity:** The execution logic is streamlined into a single readable loop, lowering the average cyclomatic complexity to 1.7.
* **Scalability:** Adding new states or events requires appending a single row to a static data table, rather than rewriting core conditional logic.
* **Reliability:** The deterministic nature of the data-driven table enables predictable behavior and facilitates 100% unit test coverage.

## Key Features
* **Table-Driven Logic:** Scalable and highly maintainable C99 implementation.
* **Non-Blocking Timeouts:** Integrates a simulated hardware timer (SysTick) to automatically safely reset the machine after 30 seconds of inactivity during the selection phase.
* **Hardware Abstraction:** Hardware inputs and outputs are stubbed, making the FSM logic directly portable to the STM32 HAL.
* **Automated Testing:** Fully validated using the Unity framework, covering nominal flows, timeouts, and error boundaries.
* **Static Analysis:** Code quality mathematically verified using the Lizard complexity analyzer.

## Project Structure
```text
FSM_Making/
├── fsm.h                  # State/Event definitions and public API
├── fsm.c                  # Transition table and core FSM engine
├── test_fsm.c             # Unity test suite and assertions
├── unity.c / unity.h      # Unity testing framework core
└── rapport_complexite.txt # Static analysis results


## Build and Test

### Requirements

* GCC compiler
* Unity Framework source files

### Compilation

```bash
gcc fsm.c test_fsm.c unity.c -o tests_fsm
```

### Execute Tests

**Windows**

```bash
tests_fsm.exe
```

**Linux / macOS**

```bash
./tests_fsm
```

---

## Technologies and Tools

| Category             | Technology              |
| -------------------- | ----------------------- |
| Programming Language | C (C99)                 |
| Embedded Platform    | STM32 (portable design) |
| Unit Testing         | Unity Framework         |
| Documentation        | Doxygen                 |
| Static Analysis      | Lizard, Cppcheck        |
| Version Control      | Git                     |
| Repository Hosting   | GitHub                  |

---

## Future Improvements

Potential extensions include:

* Addition of a maintenance mode.
* Support for multiple product categories.
* Integration with STM32 HAL peripherals.
* LCD or OLED user interface.
* Event logging through UART.
* Persistent configuration storage using EEPROM or Flash memory.

---

## License

This project is intended for educational and demonstration purposes in the field of Embedded Systems and Software Engineering.
