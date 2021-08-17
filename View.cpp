#include "View.h"

void View::get_view_positions(uint16_t* p) {
  Serial.println("Default get_view_positions()!!!");
}

void ClockView::get_view_positions(uint16_t* p) {
  // Update hour, minute
  
  if(format_24h) {
    get_view_positions(p, (hour - 1) % 12 + 1, minute);
  }
  else {
    get_view_positions(p, hour, minute);
  }
}

void ClockView::get_view_positions(uint16_t* p, uint8_t h, uint8_t m) {
  for(uint8_t digit_idx = 0; digit_idx < 4; digit_idx++) {  // digit_idx goes left to right (Hh:Mm)
    uint8_t digit;
    if(digit_idx == 0)      digit = h / 10;
    else if(digit_idx == 1) digit = (h - 1) / 10 + 1;
    else if(digit_idx == 2) digit = m / 10;
    else if(digit_idx == 3) digit = (m - 1) / 10 + 1;
    
    uint8_t clock_offset = digit_idx * CLOCK_NUMBER_DIGIT;

    for(uint8_t clock_idx = 0; clock_idx < CLOCK_NUMBER_DIGIT; clock_idx++) {
      p[clock_offset + clock_idx]                = ClockView::numbers[digit][clock_idx][0];
      p[CLOCK_NUMBER + clock_offset + clock_idx] = ClockView::numbers[digit][clock_idx][1];
    }
  }
}

const uint16_t ClockView::numbers[10][CLOCK_NUMBER_DIGIT][2] = {
/*0*/ {{Clock::hour_pos[6],Clock::hour_pos[3]}, {Clock::hour_pos[6],Clock::hour_pos[0]}, {Clock::hour_pos[3],Clock::hour_pos[0]}, {Clock::hour_pos[9],Clock::hour_pos[6]}, {Clock::hour_pos[6],Clock::hour_pos[0]}, {Clock::hour_pos[9],Clock::hour_pos[0]}},
/*1*/ {{Clock::hour_pos[14],Clock::hour_pos[14]}, {Clock::hour_pos[14],Clock::hour_pos[14]}, {Clock::hour_pos[14],Clock::hour_pos[14]}, {Clock::hour_pos[6],Clock::hour_pos[6]}, {Clock::hour_pos[6],Clock::hour_pos[0]}, {Clock::hour_pos[0],Clock::hour_pos[0]}},
/*2*/ {{Clock::hour_pos[3],Clock::hour_pos[3]}, {Clock::hour_pos[6],Clock::hour_pos[3]}, {Clock::hour_pos[3],Clock::hour_pos[0]}, {Clock::hour_pos[9],Clock::hour_pos[6]}, {Clock::hour_pos[9],Clock::hour_pos[0]}, {Clock::hour_pos[9],Clock::hour_pos[9]}},
/*3*/ {{Clock::hour_pos[3],Clock::hour_pos[3]}, {Clock::hour_pos[3],Clock::hour_pos[3]}, {Clock::hour_pos[3],Clock::hour_pos[3]}, {Clock::hour_pos[9],Clock::hour_pos[6]}, {Clock::hour_pos[9],Clock::hour_pos[0]}, {Clock::hour_pos[9],Clock::hour_pos[0]}},
/*4*/ {{Clock::hour_pos[6],Clock::hour_pos[6]}, {Clock::hour_pos[3],Clock::hour_pos[0]}, {Clock::hour_pos[14],Clock::hour_pos[14]}, {Clock::hour_pos[6],Clock::hour_pos[6]}, {Clock::hour_pos[6],Clock::hour_pos[0]}, {Clock::hour_pos[0],Clock::hour_pos[0]}},
/*5*/ {{Clock::hour_pos[6],Clock::hour_pos[3]}, {Clock::hour_pos[3],Clock::hour_pos[0]}, {Clock::hour_pos[3],Clock::hour_pos[3]}, {Clock::hour_pos[9],Clock::hour_pos[9]}, {Clock::hour_pos[9],Clock::hour_pos[6]}, {Clock::hour_pos[9],Clock::hour_pos[0]}},
/*6*/ {{Clock::hour_pos[6],Clock::hour_pos[3]}, {Clock::hour_pos[6],Clock::hour_pos[0]}, {Clock::hour_pos[3],Clock::hour_pos[0]}, {Clock::hour_pos[9],Clock::hour_pos[9]}, {Clock::hour_pos[9],Clock::hour_pos[6]}, {Clock::hour_pos[9],Clock::hour_pos[0]}},
/*7*/ {{Clock::hour_pos[3],Clock::hour_pos[3]}, {Clock::hour_pos[14],Clock::hour_pos[14]}, {Clock::hour_pos[14],Clock::hour_pos[14]}, {Clock::hour_pos[9],Clock::hour_pos[6]}, {Clock::hour_pos[6],Clock::hour_pos[0]}, {Clock::hour_pos[0],Clock::hour_pos[0]}},
/*8*/ {{Clock::hour_pos[6],Clock::hour_pos[3]}, {Clock::hour_pos[6],Clock::hour_pos[3]}, {Clock::hour_pos[3],Clock::hour_pos[0]}, {Clock::hour_pos[9],Clock::hour_pos[6]}, {Clock::hour_pos[9],Clock::hour_pos[6]}, {Clock::hour_pos[9],Clock::hour_pos[0]}},
/*9*/ {{Clock::hour_pos[6],Clock::hour_pos[3]}, {Clock::hour_pos[3],Clock::hour_pos[0]}, {Clock::hour_pos[3],Clock::hour_pos[3]}, {Clock::hour_pos[9],Clock::hour_pos[6]}, {Clock::hour_pos[6],Clock::hour_pos[0]}, {Clock::hour_pos[9],Clock::hour_pos[0]}}
};
