#include "Motor.h"

#define DEBUG_VAR_NAME(var) {Serial.print(#var); Serial.print(": "); Serial.println(var);}

Motor::Motor() : accel_max(ACCEL_MAX), speed_max(SPEED_MAX) {
    m_start_time_us = 0;
    m_delay_us = 0;
    m_travel_us = 0;
    m_travel_steps = 0;
    m_travel_dir = 0;
    m_v = 0;
    m_tr_us = 0;
    m_coast_phase = 0;

    s_last_call_us = 0;
    s_steps_at_last_call = 0;
    s_last_steps_stepped = 0;
}

bool Motor::calculateMove(int32_t travel_steps, uint32_t travel_us, uint32_t delay_us = 0, uint64_t start_time_us = micros()) {
  double v = 0;  // Coasting speed [steps/s]
  double tr_us = 0; // Speed ramp time [us]
  
  double no_coast_speed = accel_max * (travel_us / 1000000.0) / 2.0;  // Theorical speed if only accelerating then decelerating
  
  bool legal_move = true;
  
  if(no_coast_speed > speed_max) {  // Constant speed phase (aka coasting) exists
    m_coast_phase = true;
    
    v = calculateCoastSpeed(travel_us, abs(travel_steps));
    
    if(v > speed_max) { // Move is illegal (too fast)
      legal_move = false;
    }
  }
  else { // No constant speed phase (only accel/decel phases)
    m_coast_phase = false;
    
    v = no_coast_speed;
  }
  
  tr_us = v / accel_max * 1000000;
  
  if(!legal_move) { // Adjust travel time
    travel_us = 2*tr_us + 1000000.0 * (abs(travel_steps) - accel_max * pow(tr_us / 1000000.0, 2.0)) / v;
  }
  /*
  DEBUG_VAR_NAME(no_coast_speed);
  DEBUG_VAR_NAME(v);
  DEBUG_VAR_NAME(legal_move);
  DEBUG_VAR_NAME(tr_us);
  DEBUG_VAR_NAME(travel_us);
  DEBUG_VAR_NAME(travel_steps);
  */
  // Store parameters for future uses?
  m_start_time_us = start_time_us;
  m_delay_us = delay_us;
  m_travel_us = travel_us;
  m_travel_steps = abs(travel_steps);
  m_travel_dir = (travel_steps >= 0) ? 1 : -1;
  m_v = v;
  m_tr_us = tr_us;
  
  s_last_call_us = 0;
  s_steps_at_last_call = (m_travel_steps > 0) ? 0.01 : -0.01;
  s_last_steps_stepped = 0;
  
  return legal_move;
}

bool Motor::isMoveComplete() {
  return abs(s_steps_at_last_call - m_travel_steps) <= 0.01;
}

int16_t Motor::nextStep() {
  double now_us = micros();
  double now_steps = s_steps_at_last_call;
  
  // Constant Acceleration
  double phase_start_us = m_start_time_us + m_delay_us;
  double phase_end_us = m_start_time_us + m_delay_us + m_tr_us;
  
  if((s_last_call_us < phase_end_us) &&  // Previous call was before end of phase AND...
      (now_us >= phase_start_us)) {      // Now is after start of phase
    double segment_start_us = max((double)s_last_call_us, phase_start_us) - phase_start_us;
    double segment_end_us = min(now_us, phase_end_us) - phase_start_us;

    double temp_steps = (accel_max / 2.0 * pow(segment_end_us/1000000.0, 2.0)) - (accel_max / 2.0 * pow(segment_start_us/1000000.0, 2.0));
    now_steps += temp_steps;
  }

  // Constant Speed
  phase_start_us = m_start_time_us + m_delay_us + m_tr_us;
  phase_end_us = m_start_time_us + m_delay_us + m_travel_us - m_tr_us;
  
  if(m_coast_phase) {
    if((s_last_call_us < phase_end_us) &&  // Previous call was before end of phase AND...
        (now_us >= phase_start_us)) {      // Now is after start of phase
      double segment_start_us = max((double)s_last_call_us, phase_start_us) - phase_start_us;
      double segment_end_us = min(now_us, phase_end_us) - phase_start_us;
  
      double temp_steps = (m_v * (segment_end_us - segment_start_us) / 1000000);
      now_steps += temp_steps;
    }
  }

  // Constant Deceleration
  phase_start_us = m_start_time_us + m_delay_us + m_travel_us - m_tr_us;
  phase_end_us = m_start_time_us + m_delay_us + m_travel_us;
  
  if((s_last_call_us < phase_end_us) &&  // Previous call was before end of phase AND...
      (now_us >= phase_start_us)) {      // Now is after start of phase
    double segment_start_us = max((double)s_last_call_us, phase_start_us) - (m_start_time_us + m_delay_us);
    double segment_end_us = min(now_us, phase_end_us) - (m_start_time_us + m_delay_us);

    double temp_steps = (-accel_max / 2.0 * pow(segment_end_us/1000000.0, 2.0) + accel_max * m_travel_us / 1000000.0 * segment_end_us / 1000000.0) 
                        - (-accel_max / 2.0 * pow(segment_start_us/1000000.0, 2.0) + accel_max * m_travel_us / 1000000.0 * segment_start_us / 1000000.0);
    now_steps += temp_steps;
  }

  uint64_t new_steps = static_cast<uint64_t>(now_steps) - s_last_steps_stepped;
  s_last_call_us = now_us;
  s_last_steps_stepped += new_steps;
  s_steps_at_last_call = now_steps;

  return new_steps * m_travel_dir;
}

uint32_t Motor::getTravelUs() {
  return m_travel_us;
}

double Motor::calculateCoastSpeed(uint32_t travel_us, int32_t travel_steps) {
  double travel_s = travel_us / 1000000.0;
  double v = 0.5 * travel_s - sqrt(pow(travel_s, 2.0) / 4.0 - (double)travel_steps / accel_max);
  v = accel_max * v;
  
  return v;
}
