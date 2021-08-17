#include "Clock.h"

Clock::Clock(uint8_t chain_index, int32_t zero_a, int32_t zero_b) : chain_index(chain_index), pos_a(zero_a), pos_b(zero_b) {
  Serial.print("Clock #");
  Serial.print(chain_index);
  Serial.println(" online!");
}

/***
 * Prepare motors A/B to move by steps_a/b steps (positive is CW, negative is CCW)
 */
bool Clock::moveBy(int32_t steps_a, int32_t steps_b, uint32_t travel_us, uint64_t at_us) {
  bool legal_moves = true;
  legal_moves &= m_a.calculateMove(steps_a, travel_us, 0, at_us);
  legal_moves &= m_b.calculateMove(steps_b, travel_us, 0, at_us);

  return legal_moves;
}

/***
 * Prepare motors A/B to move to target_a/b in the given direction and with a minimum number of steps
 */
bool Clock::moveToSlow(int32_t target_a, int32_t target_b, uint32_t travel_us, uint32_t steps_min, bool cw_a, bool cw_b, uint64_t at_us) {
  pos_a = reducePosition(pos_a);
  pos_b = reducePosition(pos_b);

  int32_t steps_a, steps_b;

  // Handle CCW rotations as the mirror of a CW rotation from target to current position
  if(cw_a) steps_a = reducePosition(target_a - pos_a);
  else     steps_a = reducePosition(pos_a - target_a);
  if(cw_b) steps_b = reducePosition(target_b - pos_b);
  else     steps_b = reducePosition(pos_b - target_b);

  // Add some revolutions until steps_min is satisfied
  if(steps_a < steps_min) { 
    uint32_t steps_missing = steps_min - steps_a;
    steps_a += ((steps_missing / STEPS_PER_REV) + 1) * STEPS_PER_REV;
  }
  if(steps_b < steps_min) {
    uint32_t steps_missing = steps_min - steps_b;
    steps_b += ((steps_missing / STEPS_PER_REV) + 1) * STEPS_PER_REV;
  }

  // Correct the sign if CCW movement
  if(!cw_a) steps_a = -steps_a;
  if(!cw_b) steps_b = -steps_b;

  return moveBy(steps_a, steps_b, travel_us, at_us);
}

/***
 * Prepare motors A/B to move to target_a/b in the shortest direction
 */
bool Clock::moveToDirect(int32_t target_a, int32_t target_b, uint32_t travel_us, uint64_t at_us) {
  pos_a = reducePosition(pos_a);
  pos_b = reducePosition(pos_b);

  int32_t steps_a, steps_b;

  steps_a = reducePositionCentered(target_a - pos_a);
  steps_b = reducePositionCentered(target_b - pos_b);

  return moveBy(steps_a, steps_b, travel_us, at_us);
}

void Clock::updateStep(uint8_t step_index[]) {
  if(!m_a.isMoveComplete()) {
    int16_t next_step = m_a.nextStep();
    
    step_index[2*chain_index] = ((step_index[2*chain_index] + next_step) % 6 + 6) % 6;
    pos_a += next_step;
  }
  if(!m_b.isMoveComplete()) {
    int16_t next_step = m_b.nextStep();
    
    step_index[2*chain_index+1] = ((step_index[2*chain_index+1] + next_step) % 6 + 6) % 6;
    pos_b += next_step;
  }
}

uint32_t Clock::getTravelUs() {
  return max(m_a.getTravelUs(), m_b.getTravelUs());
}

bool Clock::isMoveComplete() {
  return m_a.isMoveComplete() && m_b.isMoveComplete();
}

/***
 * Reduce motor positions to the interval [0, STEPS_PER_REV) 
 */
int32_t Clock::reducePosition(int32_t pos) {
  return (pos % STEPS_PER_REV + STEPS_PER_REV) % STEPS_PER_REV;
}

/***
 * Reduce motor positions to the interval [-STEPS_PER_REV/2, STEPS_PER_REV/2) 
 */
int32_t Clock::reducePositionCentered(int32_t pos) {
  int32_t temp = reducePosition(pos);
  if(temp >= STEPS_PER_REV/2) temp -= STEPS_PER_REV;

  return temp;
}

 const  uint16_t Clock::hour_pos[16] = {0*STEPS_PER_REV/12, 1*STEPS_PER_REV/12,  2*STEPS_PER_REV/12,  3*STEPS_PER_REV/12,
                                        4*STEPS_PER_REV/12, 5*STEPS_PER_REV/12,  6*STEPS_PER_REV/12,  7*STEPS_PER_REV/12,
                                        8*STEPS_PER_REV/12, 9*STEPS_PER_REV/12, 10*STEPS_PER_REV/12, 11*STEPS_PER_REV/12,
                                        1*STEPS_PER_REV/8,  3*STEPS_PER_REV/8,   5*STEPS_PER_REV/8,   7*STEPS_PER_REV/8};
