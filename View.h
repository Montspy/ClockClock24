#ifndef VIEW_H
#define VIEW_H

#include <Arduino.h>
#include "Clock.h"

#define COLUMNS 8
#define ROWS    3
#define CLOCK_NUMBER (ROWS*COLUMNS)
#define MOTOR_NUMBER (2*CLOCK_NUMBER)
#define CLOCK_NUMBER_DIGIT 6   // Each digit is 6 clocks (12 motors)

class View {
  private:

  public:

  protected:
    virtual void get_view_positions(uint16_t* p);
    uint16_t positions[CLOCK_NUMBER * 2];
};

class ClockView : public View {
  private:
    bool format_24h;
    /***
       Motor positions for each number (10 numbers, 6 clocks per digit, 2 motors per clock)
    */
    const static uint16_t numbers[10][CLOCK_NUMBER_DIGIT][2];
    uint8_t hour, minute;

  public:
    ClockView(bool format_24h = true)
      : View(), format_24h(format_24h) {}
    void get_view_positions(uint16_t* p);
    void get_view_positions(uint16_t* p, uint8_t h, uint8_t m);
};

#endif
