#include "common.h"
#include <amdev.h>

extern size_t ramdisk_read(void*, size_t, size_t);
extern size_t ramdisk_write(const void*, size_t, size_t);
extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (int index=0; index<len; index++) _putc(*((char*)buf+index));
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  size_t event_len = 1;
  while (*(char*)(&ramdisk_start+offset+len) != '\n') event_len++;
  ramdisk_read(buf, offset, event_len);
  return 0;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
