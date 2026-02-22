/////////////////////////////////////////////////
// Splaton
// by: Jonathan Sherman and Andrew Halpern
//
////////////////////////////////////////////////////

// Control the board by getting most coloured land.
//Move up, down, right and left with the jotistick ND PUSH DOWN AND RELEASE FOR SPECIAL ABILITY
//Yellow dots mean speed, colour over them to get the speed boost
// List of parts:
// Aruidno mega, rgb matrix, two joysticks, lots of wires, box, tape, glue, cords   
//
//


#include <RGBmatrixPanel.h>
#include <Bounce2.h>
#include <Adafruit_GFX.h>

// LED display Pins
#define CLK 11
#define OE 9
#define LAT 10
#define A A0
#define B A1
#define C A2
#define D A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

// Joystick and Button Pins
int joystick1X = A8;
int joystick1Y = A9;
int button1Pin = 2;
int joystick2X = A6;
int joystick2Y = A7;
int button2Pin = 3;

#define DEAD_ZONE 100

Bounce2::Button button1, button2;

// Game Dimensions and Timing
#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 32
#define GAME_TIME 60000        // 1 minutes game time
#define ABILITY_COOLDOWN 8000  // 10 seconds cooldown for ability

// Players
int player1X = 0, player1Y = 0;
int player2X = MATRIX_WIDTH - 1, player2Y = MATRIX_HEIGHT - 1;
int x1, y1, x2, y2;

// Special Ability Timers
long player1AbilityLastUsed = 0;
long player2AbilityLastUsed = 0;
long powerupCoolDown = 0;

bool ability = false;
bool ability2 = false;
int abilityUsed = 0;
// Grid and Colors
int grid[MATRIX_WIDTH][MATRIX_HEIGHT] = { 0 };

int player1Color = matrix.Color333(255, 0, 0);       // Red
int player2Color = matrix.Color333(0, 0, 255);       // Blue
int player1Position = matrix.Color333(255, 0, 255);  // color for Player 1 position
int player2Position = matrix.Color333(0, 255, 255);  // color for Player 2 position
int obstacleColor = matrix.Color333(255, 255, 0);    // Yellow for obstacles

// Game variables
long gameStartTime;
int player1Score = 0, player2Score = 0;
int speed = 1;
int speed2 = 1;

void setup() {
  matrix.begin();
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);

  button1.attach(button2Pin);
  button2.attach(button1Pin);
  button1.interval(5);
  button2.interval(5);

  Serial.begin(9600);
  resetGrid();
  gameStartTime = millis();  // Start the timer
}

void loop() {
  button1.update();
  button2.update();

  handleInput();
  renderGrid();

  if (millis() - gameStartTime >= GAME_TIME) {
    calculateScores();
    displayWinner();
    while (true); // Infinitly pauses game
  }
}

void handleInput() {
  x1 = analogRead(joystick2X);
  y1 = analogRead(joystick2Y);
  x2 = analogRead(joystick1X);
  y2 = analogRead(joystick1Y);

  if (abs(x1 - 512) < DEAD_ZONE) x1 = 512;  // Dead zone because joyStick is too sensative
  if (abs(y1 - 512) < DEAD_ZONE) y1 = 512;

  if (abs(x2 - 512) < DEAD_ZONE) x2 = 512;
  if (abs(y2 - 512) < DEAD_ZONE) y2 = 512;

  // Player 1 Movement and Ability
  if (x1 > 612 && player1X > 0) player1X -= speed;
  else if (x1 < 412 && player1X < MATRIX_WIDTH - 1) player1X += speed;
  if (y1 > 612 && player1Y < MATRIX_HEIGHT - 1) player1Y += speed;
  else if (y1 < 412 && player1Y > 0) player1Y -= speed;

  // Player 2 Movement and Ability
  if (x2 > 612 && player2X > 0) player2X -= speed2;
  else if (x2 < 412 && player2X < MATRIX_WIDTH - 1) player2X += speed2;
  if (y2 > 612 && player2Y < MATRIX_HEIGHT - 1) player2Y += speed2;
  else if (y2 < 412 && player2Y > 0) player2Y -= speed2;

  paintAround(player1X, player1Y, 1, 2);
  paintAround(player2X, player2Y, 2, 2);

  if (millis() - powerupCoolDown > ABILITY_COOLDOWN * 2) {
    placeObstacles();
    powerupCoolDown = millis();
  }

  if (ability == true) {
    abilityUsed = millis();
    if (millis() - abilityUsed < 5000) {
      speed = 2;
    }

    else {
      speed = 1;
      ability = false;
    }
  }

  if (ability2 == true) {
    abilityUsed = millis();
    if (millis() - abilityUsed < 5000) {
      speed2 = 2;
    }

    else {
      speed2 = 1;
      ability2 = false;
    }
  }

  if (button1.pressed() && millis() - player1AbilityLastUsed > ABILITY_COOLDOWN) {
    activateAbility(player1X, player1Y, 1);
    player1AbilityLastUsed = millis();
  }
  if (button2.pressed() && millis() - player2AbilityLastUsed > ABILITY_COOLDOWN) {
    activateAbility(player2X, player2Y, 2);
    player2AbilityLastUsed = millis();
  }

  grid[player1X][player1Y] = 1;
  grid[player2X][player2Y] = 2;
}

void paintAround(int x, int y, int playerNumber, int radius) {
  int rSquared = radius * radius;
  for (int i = -radius; i <= radius; i++) {
    for (int j = -radius; j <= radius; j++) {
      if (i * i + j * j <= rSquared) {
        int newX = x + i;
        int newY = y + j;
        if (newX >= 0 && newX < MATRIX_WIDTH && newY >= 0 && newY < MATRIX_HEIGHT && grid[newX][newY] != 3) {
          grid[newX][newY] = playerNumber;
        } else if (newX >= 0 && newX < MATRIX_WIDTH && newY >= 0 && newY < MATRIX_HEIGHT && grid[newX][newY] == 3) {
          grid[newX][newY] = playerNumber;
          if (playerNumber == 1) ability = true;
          else if (playerNumber == 2) ability2 = true;
        }
      }
    }
  }
}

void activateAbility(int x, int y, int playerNumber) {
  int radius = 8;
  paintAround(x, y, playerNumber, radius);
}

void resetGrid() {
  for (int x = 0; x < MATRIX_WIDTH; x++) {
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      grid[x][y] = 0;
    }
  }
}

void placeObstacles() {
  int obsX = random(0, MATRIX_WIDTH);
  int obsY = random(0, MATRIX_HEIGHT);
  if (obsX != player1X && obsX != player2X && obsY != player1Y && obsY != player2Y) {
    paintAround(obsX, obsY, 3, 1);
  }
}

void calculateScores() {
  player1Score = 0;
  player2Score = 0;
  for (int x = 0; x < MATRIX_WIDTH; x++) {
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      if (grid[x][y] == 1) player1Score++;
      if (grid[x][y] == 2) player2Score++;
    }
  }
}

void displayWinner() {
  Serial.println("Game Over!");
  Serial.print("Player 1 Score: ");
  Serial.println(player1Score);
  Serial.print("Player 2 Score: ");
  Serial.println(player2Score);
  if (player1Score > player2Score) {
    Serial.println("Player 1 Wins!");
  } else if (player2Score > player1Score) {
    Serial.println("Player 2 Wins!");
  } else {
    Serial.println("It's a Tie!");
  }
}

void renderGrid() {
  for (int x = 0; x < MATRIX_WIDTH; x++) {
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      if (grid[x][y] == 1) matrix.drawPixel(x, y, player1Color);
      else if (grid[x][y] == 2) matrix.drawPixel(x, y, player2Color);
      else if (grid[x][y] == 3) matrix.drawPixel(x, y, obstacleColor);
    }
  }
  matrix.drawPixel(player1X, player1Y, player1Position);
  matrix.drawPixel(player2X, player2Y, player2Position);
}
