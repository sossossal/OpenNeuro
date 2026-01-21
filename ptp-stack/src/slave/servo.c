#include "servo.h"
#include <stdbool.h>
#include <stdio.h>


// Global servo instance (simplified for single slave)
static ptp_servo_t g_servo;

void ptp_servo_init(ptp_servo_t *servo) {
  // Default PI gains (tuned for typical network)
  servo->kp = 0.7; // Proportional gain
  servo->ki = 0.3; // Integral gain
  servo->integral = 0.0;
  servo->last_offset = 0;
}

int64_t ptp_servo_update(int64_t offset, int64_t delay) {
  static bool initialized = false;
  if (!initialized) {
    ptp_servo_init(&g_servo);
    initialized = true;
  }

  // PI Controller: adjustment = Kp * offset + Ki * integral(offset)
  double offset_d = (double)offset;

  // Update integral
  g_servo.integral += offset_d;

  // Calculate adjustment
  double adjustment = g_servo.kp * offset_d + g_servo.ki * g_servo.integral;

  // Anti-windup: limit integral to prevent excessive accumulation
  const double MAX_INTEGRAL = 1000000.0; // 1ms
  if (g_servo.integral > MAX_INTEGRAL)
    g_servo.integral = MAX_INTEGRAL;
  if (g_servo.integral < -MAX_INTEGRAL)
    g_servo.integral = -MAX_INTEGRAL;

  printf("[Servo] Offset: %lld ns, Integral: %.2f, Adjustment: %.2f ns\n",
         offset, g_servo.integral, adjustment);

  g_servo.last_offset = offset;

  return (int64_t)adjustment;
}
