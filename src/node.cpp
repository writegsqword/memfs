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
    auto path_chunks_v = string_split(path, "/");
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
