#include "common.h"
#include <amdev.h>
#include "klib.h"



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
  int keycode = read_key();
  if (keycode!=_KEY_NONE) {
    if ((keycode & 0x8000) != 0) {
      char temp[128];
      sprintf(temp, "kd %s\n", keyname[keycode&0xFF]);
      strncpy(buf, temp, len);
      int len_temp = strlen(temp);
      return len < len_temp? len: len_temp;
    }
    else {
      char temp[128];
      sprintf(temp, "ku %s\n", keyname[keycode&0xFF]);
      strncpy(buf, temp, len);
      int len_temp = strlen(temp);
      return len < len_temp? len: len_temp;
    }
  }
  else {
    int time = (int) uptime();
    char temp[128];
    sprintf(temp, "t %d", time);
    strncpy(buf, temp, len);
    int len_temp = strlen(temp);
    return len < len_temp? len: len_temp;
  }
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
