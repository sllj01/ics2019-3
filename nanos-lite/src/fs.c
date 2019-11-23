#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}


extern size_t ramdisk_read(void*, size_t, size_t);
extern size_t ramdisk_write(const void*, size_t, size_t);

__ssize_t fs_read(int fd, void* buf, size_t len) {
  if (fd==0) assert(0);
  if (file_table[fd].size-file_table[fd].open_offset < len) return 0;
  else {
    ramdisk_read(buf, file_table[fd].disk_offset+file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return len;
  }
}

__ssize_t fs_write(int fd, void* buf, size_t len) {
  if (fd==1||fd==2) {
    for (int index=0; index<len; index++) _putc(*((char*)buf+index));
    return len;
  }

  size_t rest = file_table[fd].size-file_table[fd].open_offset;
  if (rest < len) {
    ramdisk_write(buf, file_table[fd].disk_offset+file_table[fd].open_offset, rest);
    file_table[fd].open_offset += rest;
    return rest;
  }
  else {
    ramdisk_write(buf, file_table[fd].disk_offset+file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return len;
  }
}

int fs_open(const char* pathname, int flags, int mode) {
  for (int index=3; index<sizeof(file_table)/sizeof(Finfo); index++) {
    if (file_table[index].name == pathname) {
      // file_table[index].read = fs_read;
      // file_table[index].write = fs_write;
      file_table[index].open_offset = 0;
      return index;
    }
  }
  assert(0); //no pathname file is found
}

__off_t fs_lseek(int fd, __off_t offset, int whence) {
  switch (whence) {
    case SEEK_SET: file_table[fd].open_offset = offset; break;
    case SEEK_CUR: file_table[fd].open_offset += offset; break;
    case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  file_table[fd].read = invalid_read;
  file_table[fd].write = invalid_write;
  file_table[fd].open_offset = 0;
  return 0;
}