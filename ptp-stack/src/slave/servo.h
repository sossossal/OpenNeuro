#ifndef PTP_SERVO_H
#define PTP_SERVO_H

#include <stdint.h>

// PI Controller for clock servo
typedef struct {
  double kp; // Proportional gain
  double ki; // Integral gain

  double integral; // Accumulated error
  int64_t last_offset;
} ptp_servo_t;

// Initialize servo with default gains
void ptp_servo_init(ptp_servo_t *servo);

// Update servo and return adjustment value
// offset: current clock offset from master (ns)
// delay: mean path delay (ns)
// returns: adjustment to apply to local clock (ns)
int64_t ptp_servo_update(int64_t offset, int64_t delay);

#endif // PTP_SERVO_H
