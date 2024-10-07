#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include "filesystem.h"
#include "utils.h"

static const char *filepath = "/file";
static const char *filename = "file";
static const char *filecontent = "I'm the content of the only file available there\n";

#define NONNULL_GUARD(node)                       \
  if (node == nullptr)                            \
    return -ENOENT;
  

#define _GET_FNODE(path)                          \
  FS::Node *node = FS::Node::find_recurse(path);  \
  NONNULL_GUARD(node)
  


static int getattr_callback(const char *path, struct stat *stbuf)
{
  memset(stbuf, 0, sizeof(struct stat));

  _GET_FNODE(path);
  return node->getattr(stbuf);
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{

  (void)offset;
  (void)fi;
  // navigate
  _GET_FNODE(path);
  return node->readdir(buf, filler, offset, fi);
}

static int open_callback(const char *path, struct fuse_file_info *fi)
{
  return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  _GET_FNODE(path);
  return node->read(buf, size, offset, fi);

  // return -ENOENT;
}

static int write_callback(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  _GET_FNODE(path);
  return node->write(buf, size, offset, fi);
}

static int mkdir_callback(const char *path, mode_t mode)
{
  path_chunks_t path_c = get_path_chunks(path);
  FS::Node *node = FS::Node::find_recurse_parent(path);
  NONNULL_GUARD(node);
  if(!node->isdir())
    return -ENOENT;
  FS::Directory* dirnode = (FS::Directory*)node;
  
  dirnode->addentry(path_c.back(), new FS::Directory());
  
  return 0;
}

static struct fuse_operations fuse_operations = {
    .getattr = getattr_callback,
    .mkdir = mkdir_callback,
    .open = open_callback,
    .read = read_callback,
    .write = write_callback,
    .readdir = readdir_callback,

};

int main(int argc, char *argv[])
{
  FS::Directory *pdir = new FS::Directory();
  FS::File *f = new FS::File();
  const char buf[] = "hello i am file";
  f->write(buf, sizeof(buf), 0, 0);
  std::string ename("lol");
  pdir->addentry(ename, f);

  FS::Node::root = pdir;
  return fuse_main(argc, argv, &fuse_operations, NULL);
}