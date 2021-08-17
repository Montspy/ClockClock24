#include <SPI.h>
#include "Clock.h"
#include "View.h"

const uint8_t step_sequence[6] = {0b00001001, 0b00000001, 0b00000111, 0b00000110, 0b00001110, 0b00001000};

#define PIN_MOSI  18
#define PIN_SCLK  5
#define PIN_CLR   16
#define PIN_LATCH 17
#define PIN_OE    21

uint8_t step_index[MOTOR_NUMBER] = {};
Clock* clocks[CLOCK_NUMBER];

SPISettings spi_settings(1000000, MSBFIRST, SPI_MODE0);
ClockView cview;
uint16_t positions[CLOCK_NUMBER * 2] = {};

uint64_t mic = micros();
int8_t inc = 0;

void setup() {
  Serial.begin(115200);

  init_clocks();

  init_shiftreg();
}

void loop() {
  delayMicroseconds(1000);

  bool allMovesComplete = true;
  for(uint8_t i = 0; i < CLOCK_NUMBER; i++) {
    clocks[i]->updateStep(step_index);

    allMovesComplete &= clocks[i]->isMoveComplete();
  }
  
  shift_out(step_index, MOTOR_NUMBER);

  if(allMovesComplete) {
    mic = micros();

    cview.get_view_positions(positions, inc * 10, 0);
    Serial.print("Moving to number ");
    Serial.println(inc);
    
    for(uint8_t i = 0; i < CLOCK_NUMBER; i++) {
      clocks[i]->moveToDirect(positions[i], positions[CLOCK_NUMBER + i], 10*1000000, mic + 1000000);
    }

    inc = (inc + 1) % 10;
  }
}

void init_clocks() {
  for(uint8_t i = 0; i < CLOCK_NUMBER; i++) {
    uint8_t chain_index = i;

    if((i / ROWS) % 2 == 1) { // Every other clock column, invert chain direction
      chain_index = ((i / ROWS) + 1) * 3 - 1 - (i % ROWS);
    }
    clocks[i] = new Clock(chain_index);
    clocks[i]->moveToDirect(0, 0, 10*1000000, mic + 1000000);
  }
}

void init_shiftreg() {
  pinMode(PIN_CLR, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_OE, OUTPUT);

  digitalWrite(PIN_CLR, HIGH);
  digitalWrite(PIN_LATCH, LOW);
  digitalWrite(PIN_OE, HIGH);

  SPI.begin();

  // Clear registers and shift out
  digitalWrite(PIN_CLR, LOW);
  delayMicroseconds(10);
  digitalWrite(PIN_CLR, HIGH);
  digitalWrite(PIN_LATCH, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_LATCH, LOW);

  shift_out(step_index, MOTOR_NUMBER);
  
  digitalWrite(PIN_OE, LOW);
}

void shift_out(uint8_t* step_index_out, uint8_t motor_number) {
  // Shift out
  SPI.beginTransaction(spi_settings);
  
  for(uint8_t i = 0; i < motor_number/2; i++) {
    // Combine A motor and B motor step sequences in 1 byte, last motor first (invert A motors direction)
    uint8_t inv_i = motor_number - 2*(i + 1);
    byte buf = (step_sequence[5-step_index[inv_i]]) + (step_sequence[step_index[inv_i+1]] << 4);

    SPI.transfer(buf);
  }
  
  SPI.endTransaction();

  // Latch
  digitalWrite(PIN_LATCH, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_LATCH, LOW);
}
