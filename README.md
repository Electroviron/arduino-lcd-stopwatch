# Arduino LCD Stopwatch ⏱️

A simple digital stopwatch built with an **Arduino Uno**, a **16×2 character LCD**, and three push buttons.

This project was built as a hands-on exercise for learning how LCDs, digital inputs, button state detection, timing with `millis()`, and basic embedded-system state management work together.

> **Note:** This is a learning project. The current implementation works, but the code is not yet considered a fully optimized or production-quality stopwatch. Further improvements will be made as more embedded concepts are learned.

---

## 📌 Features

* 16×2 LCD display
* Start button
* Stop button
* Reset button
* Seconds and minutes tracking
* `millis()`-based timing
* Button edge detection
* Arduino internal pull-up resistors
* Two-digit formatting (`00–59`)
* Automatic rollover from seconds to minutes

Example display:

```text
Timer->00:00
```

While running:

```text
Timer->01:37
```

---

# 🧰 Components

### Hardware

* Arduino Uno
* 16×2 LCD (HD44780-compatible)
* 3 × push buttons
* Jumper wires
* Breadboard
* 10 kΩ potentiometer for LCD contrast
* USB cable / power source

### Software

* Arduino IDE
* Arduino `LiquidCrystal` library

---

# 🔌 Wiring

## LCD

The LCD is operated in **4-bit mode**, so only D4–D7 are used for data.

| LCD Pin | Function        | Arduino             |
| ------- | --------------- | ------------------- |
| VSS     | Ground          | GND                 |
| VDD     | +5 V            | 5V                  |
| V0      | Contrast        | Potentiometer wiper |
| RS      | Register Select | D9                  |
| RW      | Read/Write      | GND                 |
| E       | Enable          | D8                  |
| D4      | Data            | D4                  |
| D5      | Data            | D5                  |
| D6      | Data            | D6                  |
| D7      | Data            | D7                  |
| A       | Backlight +     | 5V*                 |
| K       | Backlight −     | GND                 |

* The required backlight current-limiting arrangement depends on the particular LCD module.

## Buttons

| Button | Arduino Pin | Configuration  |
| ------ | ----------: | -------------- |
| Start  |         D12 | `INPUT_PULLUP` |
| Reset  |         D11 | `INPUT_PULLUP` |
| Stop   |         D10 | `INPUT_PULLUP` |

Each button is connected between its Arduino input pin and **GND**.

Because `INPUT_PULLUP` is used:

```text
Button released → HIGH
Button pressed  → LOW
```

---

# 🧠 How the LCD Works

The LCD uses several groups of pins.

### Power

```text
VSS → GND
VDD → +5V
V0  → Contrast control
```

### Control

```text
RS → Determines command/data
RW → Read/write control
E  → Enable/capture signal
```

For this project, RW is permanently connected to GND because the Arduino only needs to **write** to the LCD.

### Data

The LCD supports an 8-bit data interface through D0–D7, but this project uses **4-bit mode**:

```text
D4
D5
D6
D7
```

This saves Arduino I/O pins while still allowing the LCD to operate normally.

---

# 💻 Code Structure

The LCD is initialized with:

```cpp
LiquidCrystal lcd(9, 8, 4, 5, 6, 7);
```

The arguments correspond to:

```text
RS, E, D4, D5, D6, D7
```

The LCD is then configured as a 16×2 display:

```cpp
lcd.begin(16, 2);
```

---

# ⏱️ Stopwatch Timing

The stopwatch uses:

```cpp
millis()
```

instead of `delay()`.

`millis()` returns the number of milliseconds that have elapsed since the Arduino started running.

The program checks:

```cpp
if ((current_tms - initial_tms) >= 1000)
```

This means:

> Has at least one second passed?

When one second has passed:

```cpp
current_ts++;
```

The seconds counter increases.

When seconds reach 60:

```cpp
if (current_ts >= 60)
{
    current_ts = 0;
    current_tm++;
}
```

The seconds roll over to zero and the minute counter increases.

Therefore:

```text
00:58
00:59
01:00
01:01
```

---

# 🎛️ Button State Detection

The buttons use edge detection rather than simply checking whether a button is currently pressed.

For example:

```cpp
if(startButtonState == LOW && lastStartButtonState == HIGH)
{
    running = true;
}
```

This detects the transition:

```text
HIGH → LOW
```

which represents a new button press.

The previous state is stored at the end of every loop:

```cpp
lastStartButtonState = startButtonState;
```

This allows the program to distinguish between:

```text
NEW PRESS
HIGH → LOW
```

and:

```text
BUTTON STILL HELD
LOW → LOW
```

---

# 🧩 Stopwatch State

The stopwatch uses:

```cpp
bool running = false;
```

This variable represents the current state of the stopwatch.

### Start

```cpp
running = true;
```

### Stop

```cpp
running = false;
```

### Reset

```cpp
running = false;
current_ts = 0;
current_tm = 0;
```

The timing code only runs when:

```cpp
if(running)
```

is true.

This separates **button input** from the actual stopwatch operation.

Conceptually:

```text
             BUTTONS
                │
       ┌────────┼────────┐
       ↓        ↓        ↓
     START    STOP     RESET
       │        │        │
       └────────┼────────┘
                ↓
          running state
                ↓
        Stopwatch timing
                ↓
               LCD
```

---

# 🐛 Problems Encountered

This project was intentionally developed through experimentation, so several problems appeared during development.

## 1. `counter + 1` did not increment the variable

An early LCD counter used:

```cpp
counter + 1;
```

This did not actually modify `counter`.

The important distinction was discovered:

```cpp
counter + 1;
```

calculates a value but does not store it.

Whereas:

```cpp
counter++;
```

actually increments the variable.

Equivalent form:

```cpp
counter = counter + 1;
```

This introduced the difference between **an expression that calculates a value** and **an operation that modifies a variable**.

---

## 2. LCD digits remained on the screen

When changing from a two-digit number to a one-digit number, an old digit could remain visible.

For example:

```text
10
9
```

could appear as:

```text
90
```

because writing `9` does not automatically erase the previous `0`.

The solution was to overwrite the unused character with a space or explicitly format the number using two positions.

This demonstrated an important property of character LCDs:

> Writing new characters does not automatically erase old characters.

---

## 3. One button press caused multiple counts

The first button counter sometimes increased several times from one physical press.

The cause was **mechanical button bounce**.

A real push button doesn't always produce a perfectly clean electrical transition. The contacts can rapidly alternate between states for a short period.

Conceptually:

```text
Ideal:

HIGH ────────────┐
                 └──────── LOW


Real:

HIGH ────────────┐
                 ├─┐
                 │ └─┐
                 │   └─┐
                 │     └──── LOW
```

A debounce delay was temporarily used to allow the signal to settle.

This was the first introduction to **switch debouncing**.

---

## 4. `millis()` was initially misunderstood

An early version declared:

```cpp
unsigned long current_tms = millis();
```

and expected it to continuously represent the current time.

However, that assignment happens only once.

The value needs to be updated continuously:

```cpp
current_tms = millis();
```

inside `loop()`.

This demonstrated an important distinction between:

* assigning a value once
* continuously sampling a changing value

---

## 5. Timer comparison using `==`

The first timing approach used:

```cpp
current_tms - initial_tms == 1000
```

This can fail because the loop may not execute at exactly 1000 ms.

For example, the program might observe:

```text
999 ms
1002 ms
```

without ever observing exactly:

```text
1000 ms
```

Therefore:

```cpp
current_tms - initial_tms >= 1000
```

is more appropriate.

---

## 6. Start-button logic initially detected both edges

An early condition checked both:

```text
HIGH → LOW
```

and:

```text
LOW → HIGH
```

This meant the function could execute both when the button was pressed and when it was released.

The logic was changed to detect only:

```cpp
startButtonState == LOW &&
lastStartButtonState == HIGH
```

This introduced the concept of **edge detection**.

---

# 📚 Concepts Learned

This project covered several important embedded-programming concepts:

### Electronics

* LCD pin functions
* LCD power and contrast
* LCD control signals
* 4-bit LCD communication
* Digital button inputs
* Internal pull-up resistors
* Mechanical switch bounce

### C/C++

* Variables
* `int`
* `unsigned long`
* `bool`
* `++`
* Conditional statements
* Functions
* State variables

### Arduino

* `pinMode()`
* `digitalRead()`
* `millis()`
* `lcd.begin()`
* `lcd.print()`
* `lcd.setCursor()`
* `INPUT_PULLUP`

### Embedded Systems

* Event detection
* State management
* Non-blocking timing
* Timer rollover
* Separating input handling from system behavior
* Software handling of imperfect physical hardware

---

# 🚧 Future Improvements

The current implementation works, but there are several things that can be improved.

### Planned improvements

* [ ] Implement proper non-blocking button debouncing
* [ ] Improve stopwatch timing accuracy
* [ ] Clean up variable names
* [ ] Separate button handling from stopwatch timing more clearly
* [ ] Improve LCD formatting
* [ ] Handle `millis()` rollover correctly
* [ ] Refactor the stopwatch into a cleaner state machine
* [ ] Add pause/resume functionality
* [ ] Add lap timing
* [ ] Eventually remove the `LiquidCrystal` abstraction and control the LCD directly

The final goal is not simply to have a working stopwatch, but to use the project as a stepping stone toward understanding how the LCD and Arduino communicate at a lower level.

---

# 🎯 What This Project Taught Me

This project started as a simple LCD exercise but introduced several concepts that are fundamental to embedded systems.

The most important lesson was that **hardware and software have to be considered together**.

A button isn't simply:

```text
Pressed / Not pressed
```

It is a physical device producing an electrical signal that software has to interpret.

Likewise, an LCD isn't simply:

```text
lcd.print("Hello");
```

Behind the library are control signals, data lines, timing, and a communication protocol.

This project is therefore part of a larger progression toward understanding embedded systems at a deeper level.

---

## 📝 Current Status

**Working prototype — Version 1**

The stopwatch currently supports:

```text
START
  ↓
RUN
  ↓
STOP
  ↓
RESET
```

with minute and second display on a 16×2 LCD.

Further refinement will be done as more embedded concepts are learned.
