#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include "filesystem.h"
#include "utils.h"

#define NONNULL_GUARD(node)                       \
  if (node == nullptr)                            \
    return -ENOENT;

#define ISDIR_GUARD(node) if(!node->isdir()) return -ENOENT;
#define ISDIR_GUARD_CAST(node) ISDIR_GUARD(node) FS::Directory* dirnode = (FS::Directory*)node;
#define ISFILE_GUARD(node) if(node->isdir()) return -ENOENT;
#define ISFILE_GUARD_CAST(node) ISFILE_GUARD(node) FS::File* fnode = (FS::File*)node;

#define _GET_FNODE(path)                          \
  FS::Node *node = FS::Node::find_recurse(path);  \
  NONNULL_GUARD(node)
  
#define _GET_FNODE_PARENT(path) path_chunks_t path_c = get_path_chunks(path); std::string child_name = path_c.back(); FS::Node *node = FS::Node::find_recurse_parent(path); NONNULL_GUARD(node);

#define _GET_FNODE_PARENT_CAST(path) _GET_FNODE_PARENT(path); ISDIR_GUARD_CAST(node);

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
  _GET_FNODE(path);
  return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  _GET_FNODE(path);
  return node->read(buf, size, offset, fi);

  // return -ENOENT;
}


static int truncate_callback(const char *path, off_t size)
{
  _GET_FNODE(path);
  if(node->isdir())
    return -ENOENT;
  FS::File* fnode = (FS::File*)node;
  return fnode->truncate(size);

  // return -ENOENT;
}

static int write_callback(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  _GET_FNODE(path);
  return node->write(buf, size, offset, fi);
}

static int mkdir_callback(const char *path, mode_t mode)
{
  _GET_FNODE_PARENT_CAST(path);
  
  dirnode->addentry(child_name, new FS::Directory());
  
  return 0;
}

static int create_callback(const char* path, mode_t mode, struct fuse_file_info *){
  _GET_FNODE_PARENT_CAST(path);
  dirnode->addentry(child_name, new FS::File());
  return 0;
}

static int access_callback(const char* path, int mask){
  _GET_FNODE(path);
  return 0;
}


static int unlink_callback(const char* path){
  _GET_FNODE_PARENT_CAST(path);

  FS::Node* child = dirnode->get_entry(child_name);
  NONNULL_GUARD(child);
  ISFILE_GUARD(child);
  dirnode->remove_entry(child_name);
  delete child;
  return 0;
}

static int rmdir_callback(const char* path){
  _GET_FNODE_PARENT_CAST(path);
  FS::Node* child = dirnode->get_entry(child_name);
  NONNULL_GUARD(child);
  ISDIR_GUARD(child);
  dirnode->remove_entry(child_name);
  delete child;
  return 0;
}




static struct fuse_operations fuse_operations = {
    .getattr = getattr_callback,
    .mkdir = mkdir_callback,
    .unlink = unlink_callback,
    .rmdir = rmdir_callback,
    .truncate = truncate_callback,
    .open = open_callback,
    .read = read_callback,
    .write = write_callback,
    .readdir = readdir_callback,
    .access = access_callback,
    .create = create_callback,
    

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