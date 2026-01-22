#ifndef PTP_UTILS_H
#define PTP_UTILS_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculate checksum for PTP messages
 */
uint16_t ptp_calculate_checksum(const uint8_t *data, size_t len);

/**
 * @brief Compare two timestamps
 * @return -1 if t1 < t2, 0 if equal, 1 if t1 > t2
 */
int ptp_compare_timestamps(uint64_t t1, uint64_t t2);

/**
 * @brief Convert nanoseconds to seconds and nanoseconds
 */
void ptp_ns_to_timespec(uint64_t ns, uint32_t *sec, uint32_t *nsec);

/**
 * @brief Convert seconds and nanoseconds to total nanoseconds
 */
uint64_t ptp_timespec_to_ns(uint32_t sec, uint32_t nsec);

/**
 * @brief Print timestamp in human-readable format
 */
void ptp_print_timestamp(uint64_t ns);

/**
 * @brief Safe memory copy with bounds checking
 */
int ptp_safe_memcpy(void *dest, size_t dest_size, const void *src,
                    size_t src_size);

#ifdef __cplusplus
}
#endif

#endif // PTP_UTILS_H
