# Arduino Territory Paint Game

**A two-player, Splatoon-inspired territory game running on an Arduino Mega and a 64×32 RGB LED matrix.**

## How it plays

Each player moves a cursor with a joystick and continuously paints nearby pixels in their color. Players can repaint the opponent's territory, collect yellow speed boosts, and press the joystick button to cover a much larger circle. After one minute, the sketch counts the red and blue pixels and prints the winner to the Serial Monitor.

## Features

- Simultaneous two-player joystick control
- Circular territory painting
- Red-versus-blue score calculation
- Random yellow speed power-ups
- Large-area special ability
- Eight-second ability cooldown
- One-minute timed matches
- Joystick dead zones and debounced buttons

## Hardware

- Arduino Mega
- 64×32 HUB75 RGB LED matrix
- Two analog joysticks with push buttons
- Jumper wires and a suitable external matrix power supply

## Libraries

- Adafruit GFX Library
- RGBmatrixPanel
- Bounce2

## Main pin configuration

| Component | Pins |
| --- | --- |
| Matrix CLK / OE / LAT | `11` / `9` / `10` |
| Matrix address A–D | `A0`–`A3` |
| Player 1 joystick | `A8`, `A9`, button `2` |
| Player 2 joystick | `A6`, `A7`, button `3` |

## Upload and play

1. Open `SplatoonGame/SplatoonGame.ino` in the Arduino IDE.
2. Install the three required libraries.
3. Select the Arduino Mega and upload the sketch.
4. Open the Serial Monitor at `9600` baud to see the final scores.
5. Move with the joysticks and press them to activate the splash ability.

## Game rules

| Color | Meaning |
| --- | --- |
| Red | Player 1 territory |
| Blue | Player 2 territory |
| Magenta / cyan | Current player positions |
| Yellow | Speed power-up |

The game stops after the timer expires and reports Player 1, Player 2, or a tie.
