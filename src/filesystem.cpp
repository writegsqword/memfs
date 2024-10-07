#include "filesystem.h"
#include "utils.h"
#include <iostream>
using namespace FS;


Node* Node::_find_recurse(path_chunks_t& path_chunks) {
    //if no path chunks are left, then this is the file we are looking for
    if(path_chunks.size() == 0){
        return this;    
    }
    //if not, check if this is a directory
    if(this->type != FiletypeEnum::DIR) {
        //invalid
        return nullptr;
    }
    //I know this looks dumb as fuck but i dont have a better way
    Directory* _this = (Directory*)this;
    Node* next = _this->get_entry(path_chunks.front());
    if(next == nullptr)
        return next;
    path_chunks.pop();
    return next->_find_recurse(path_chunks);
}

Node* Node::find_recurse(std::string& path){
    auto path_chunks_v = split(path, "/");
    path_chunks_t path_chunks;
    for(std::string& s : path_chunks_v){
        if(s.length() > 0)
            path_chunks.push(s);

    }
    std::cerr << path_chunks.size() << "\n";
    return root->_find_recurse(path_chunks);

}
int Node::getattr(struct stat *stbuf)
{
    memset(stbuf, 0, sizeof(struct stat));
    if (this->type == FiletypeEnum::DIR)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    if (this->type == FiletypeEnum::FILE)
    {
        stbuf->st_mode = S_IFREG | 0777;
        stbuf->st_nlink = 1;
        stbuf->st_size = fsize;
        return 0;
    }

    return -ENOENT;
}

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

int File::read(char *buf, size_t size, off_t offset,
               struct fuse_file_info *fi)
{
    size_t len = content.size();

    if (offset >= content.size())
    {
        return 0;
    }

    if (offset + size > len)
    {
        memcpy(buf, content.data() + offset, len - offset);
        return len - offset;
    }

    memcpy(buf, content.data() + offset, size);
    return size;
}

int File::write(const char *buf, size_t buf_size, off_t offset, struct fuse_file_info *fi)
{
    size_t new_size = offset + buf_size;
    new_size = std::max(new_size, content.size());
    content.resize(new_size);

    for (size_t i = 0; i < 16; i++)
    {
        size_t dst = i + offset;
        content[i] = buf[i];
    }
    return buf_size;
}

int File::getattr(struct stat *stbuf)
{
    memset(stbuf, 0, sizeof(struct stat));

    stbuf->st_mode = S_IFREG | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_size = content.size();
    return 0;

}

File::File() {
    type = FiletypeEnum::FILE;
}

Directory::Directory() {
    type = FiletypeEnum::DIR;
}