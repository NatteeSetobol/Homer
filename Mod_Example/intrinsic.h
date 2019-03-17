#ifndef __INTRINSIC__
#define __INTRINSIC__

#include <stdint.h>

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define ui8 uint8_t
#define ui16 uint16_t
#define ui32 uint32_t
#define ui64 uint64_t

#define bool32 ui32

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef bool
#define bool ui32 
#endif



#endif
