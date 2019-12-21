#include "common.h"
#include <amdev.h>
#include "klib.h"


size_t serial_write(const void *buf, size_t offset, size_t len) {
  
  // for simulation  of the block
  // _yield();
  
  for (int index=0; index<len; index++) _putc(*((char*)buf+index));
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};


int fg_pcb = 1;

size_t events_read(void *buf, size_t offset, size_t len) {

  // for simulation the block
  // _yield();

  int keycode = read_key();
  char temp[128];
  if (keycode!=_KEY_NONE) {
    if ((keycode & 0x8000) != 0) {
      sprintf(temp, "kd %s\n", keyname[keycode&0xFF]);
      strncpy(buf, temp, len);
      if (strcmp("kd F1\n", temp)==0) fg_pcb = 1;
      else if (strcmp("kd F2\n", temp)==0) fg_pcb = 2;
      else if (strcmp("kd F3\n", temp)==0) fg_pcb = 3;
    }
    else {
      sprintf(temp, "ku %s\n", keyname[keycode&0xFF]);
      strncpy(buf, temp, len);
    }
  }
  else {
    uint32_t time = uptime();
    sprintf(temp, "t %d\n", time);
    strncpy(buf, temp, len);
  }
  int len_temp = strlen(temp);
  return len < len_temp? len: len_temp;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  memcpy(buf, (void*) dispinfo+offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {

  // for simulation  of the block
  // _yield();

  int x = offset / 4 % screen_width();
  int y = (offset /4) / screen_width();
  assert(len/4 <= screen_width());
  draw_rect((uint32_t*) buf, x, y, len/4, 1);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  int width = screen_width();
  int height = screen_height();
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", width, height);
}
