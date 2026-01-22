#include <stdint.h>
#include <stdio.h>
#include <string.h>


// Common utility functions for PTP stack

/**
 * @brief Calculate checksum for PTP messages
 */
uint16_t ptp_calculate_checksum(const uint8_t *data, size_t len) {
  uint32_t sum = 0;

  for (size_t i = 0; i < len; i++) {
    sum += data[i];
  }

  return (uint16_t)(sum & 0xFFFF);
}

/**
 * @brief Compare two timestamps
 * @return -1 if t1 < t2, 0 if equal, 1 if t1 > t2
 */
int ptp_compare_timestamps(uint64_t t1, uint64_t t2) {
  if (t1 < t2)
    return -1;
  if (t1 > t2)
    return 1;
  return 0;
}

/**
 * @brief Convert nanoseconds to seconds and nanoseconds
 */
void ptp_ns_to_timespec(uint64_t ns, uint32_t *sec, uint32_t *nsec) {
  *sec = (uint32_t)(ns / 1000000000ULL);
  *nsec = (uint32_t)(ns % 1000000000ULL);
}

/**
 * @brief Convert seconds and nanoseconds to total nanoseconds
 */
uint64_t ptp_timespec_to_ns(uint32_t sec, uint32_t nsec) {
  return ((uint64_t)sec * 1000000000ULL) + nsec;
}

/**
 * @brief Print timestamp in human-readable format
 */
void ptp_print_timestamp(uint64_t ns) {
  uint32_t sec, nsec;
  ptp_ns_to_timespec(ns, &sec, &nsec);
  printf("%u.%09u s\n", sec, nsec);
}

/**
 * @brief Safe memory copy with bounds checking
 */
int ptp_safe_memcpy(void *dest, size_t dest_size, const void *src,
                    size_t src_size) {
  if (dest == NULL || src == NULL) {
    return -1;
  }

  if (src_size > dest_size) {
    return -1;
  }

  memcpy(dest, src, src_size);
  return 0;
}
