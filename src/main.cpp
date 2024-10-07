#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include "filesystem.h"

static const char *filepath = "/file";
static const char *filename = "file";
static const char *filecontent = "I'm the content of the only file available there\n";

#define _GET_FNODE std::string spath(path); FS::Node* node = FS::Node::find_recurse(spath); if( node == nullptr ) return -ENOENT;

static int getattr_callback(const char *path, struct stat *stbuf) {
  memset(stbuf, 0, sizeof(struct stat));

  _GET_FNODE;
  return node->getattr(stbuf);

}



static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) {
  
  (void) offset;
  (void) fi;
  //navigate
  _GET_FNODE;
  return node->readdir(buf, filler, offset, fi);

}

static int open_callback(const char *path, struct fuse_file_info *fi) {
  return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
  _GET_FNODE;
  return node->read(buf, size, offset, fi);


  //return -ENOENT;
}

static int write_callback(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
  _GET_FNODE;
  return node->write(buf, size, offset, fi);
}

static struct fuse_operations fuse_operations = {
  .getattr = getattr_callback,
  .open = open_callback,
  .read = read_callback,
  .write = write_callback,
  .readdir = readdir_callback,
  
};

int main(int argc, char *argv[])
{  
  FS::Directory* pdir = new FS::Directory();
  FS::File* f = new FS::File();
  const char buf[] = "hello i am file";   
  f->write(buf, sizeof(buf), 0, 0);
  std::string ename("lol");
  pdir->addentry(ename, f);

  FS::Node::root = pdir;
  return fuse_main(argc, argv, &fuse_operations, NULL);
}