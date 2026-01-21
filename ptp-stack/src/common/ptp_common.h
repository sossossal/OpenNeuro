#ifndef PTP_COMMON_H
#define PTP_COMMON_H

#include <stdint.h>

// Endianness macros
// PTP uses Network Byte Order (Big Endian)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define HTONS(x) ((uint16_t)((((x) & 0xff) << 8) | (((x) >> 8) & 0xff)))
#define HTONL(x)                                                               \
  ((uint32_t)((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) |                   \
              (((x) >> 8) & 0xff00) | (((x) >> 24) & 0xff)))
// 64-bit swap not strictly standard but needed for timestamps
#define HTONLL(x) ((((uint64_t)HTONL(x)) << 32) | HTONL((x) >> 32))
#else
#define HTONS(x) (x)
#define HTONL(x) (x)
#define HTONLL(x) (x)
#endif

#define NTOHS(x) HTONS(x)
#define NTOHL(x) HTONL(x)
#define NTOHLL(x) HTONLL(x)

#endif // PTP_COMMON_H
