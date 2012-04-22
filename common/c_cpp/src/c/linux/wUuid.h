

#ifndef WUUID_H__
#define WUUID_H__

#include <uuid/uuid.h>

typedef uuid_t wUuid;

#define wUuid_generate_time uuid_generate_time

#define wUuid_unparse uuid_unparse

#endif /* WUUID_H__ */
