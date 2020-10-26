#ifndef __HANDLE_H__
#define __HANDLE_H__

#include <bfd.h>

bfd *raw_handle(char *file);

bfd *validated_handle(bfd *handle);

#endif

