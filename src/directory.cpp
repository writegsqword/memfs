#include "filesystem.h"
#include "utils.h"
#include <iostream>
using namespace FS;

int Directory::readdir(void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    (void)offset;
    (void)fi;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    for (const auto &f : files)
    {
        filler(buf, f.first.c_str(), NULL, 0);
    }

    return 0;
}
int Directory::addentry(const std::string& name, Node *node)
{
    // Destruct previous node if exists or else this leaks memory
    files.emplace(name, node);
    return 0;
}

Node* Directory::get_entry(const std::string& name){
    //i hope the compiler optimizes this cuz im lazy
    if(files.find(name) == files.end())
        return nullptr;
    return files.find(name)->second;
}

int Directory::getattr(struct stat *stbuf)
{
    memset(stbuf, 0, sizeof(struct stat));

    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    return 0;

}


Directory::Directory() {
    type = FiletypeEnum::DIR;
}