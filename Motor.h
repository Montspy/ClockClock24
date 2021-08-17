#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

#define ACCEL_MAX 200
#define SPEED_MAX 200 // Hz or steps/s

class Motor {
  private:
    const uint16_t accel_max;
    const uint16_t speed_max;

    uint64_t m_start_time_us;
    uint32_t m_delay_us;
    uint32_t m_travel_us;
    int32_t m_travel_steps;
    int8_t m_travel_dir;
    double m_v;
    double m_tr_us;
    bool m_coast_phase;

    uint64_t s_last_call_us;        // Time of last call of nextStep()
    double s_steps_at_last_call;    // Travel steps at time of last call of nextStep()
    int64_t s_last_steps_stepped;  // Integer steps already returned by nextStep()

    double calculateCoastSpeed(uint32_t travel_us, int32_t travel_steps);
  
  public:
    Motor();
    bool calculateMove(int32_t travel_steps, uint32_t travel_us, uint32_t delay_us, uint64_t start_time_us);
    bool isMoveComplete();
    int16_t nextStep();
    uint32_t getTravelUs();
};

#endif
