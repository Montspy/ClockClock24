#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>
#include "Motor.h"

#define STEPS_PER_REV (360*3)

class Clock {
  private:
    uint8_t chain_index;
    
    Motor m_a, m_b;
    int32_t pos_a, pos_b;

    int32_t reducePosition(int32_t pos);
    int32_t reducePositionCentered(int32_t pos);
  
  public:
    Clock(uint8_t chain_index, int32_t zero_a = Clock::hour_pos[6], int32_t zero_b = Clock::hour_pos[6]);
    bool moveBy(int32_t steps_a, int32_t steps_b, uint32_t travel_us, uint64_t at_us = micros());
    bool moveToSlow(int32_t target_a, int32_t target_b, uint32_t travel_us, uint32_t steps_min = STEPS_PER_REV, bool cw_a = true, bool cw_b = false, uint64_t at_us = micros());
    bool moveToDirect(int32_t target_a, int32_t target_b, uint32_t travel_us, uint64_t at_us = micros());
    void updateStep(uint8_t step_index[]);
    uint32_t getTravelUs();
    bool isMoveComplete();

    /***
     * hour_pos[0..11]  = step position for hours 12, 1, ..., 11
     * hour_pos[12..15] = step positions for NE, SE, SW, NW direction
     */
    const static uint16_t hour_pos[16];
};

#endif
