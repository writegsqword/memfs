#pragma once
#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <string.h>
#include <errno.h>

#include <typedefs.h>

// TODO: consider using a faster impl
#include <unordered_map>

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <queue>
namespace FS
{
    namespace FiletypeEnum
    {
        enum FileType
        {
            FILE,
            DIR
        };
    }
    class Node
    {
        
    protected:
        FiletypeEnum::FileType type;
        int flags = 0;
        size_t fsize = 0;
        
        Node* _find_recurse(path_chunks_t& path_chunks);
    public:
        
        inline static Node *root;
        
        static Node* find_recurse(const std::string& path);
        static Node* find_recurse_parent(const std::string& path);
        inline bool isdir() { return type == FiletypeEnum::DIR; };
        virtual int getattr(struct stat *stbuf);
        virtual int read(char *buf, size_t size, off_t offset, struct fuse_file_info *fi) { return 0; };
        virtual int readdir(void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) { return 0; };
        virtual int write(const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) { return 0; };
        virtual ~Node();

    
    };


    class Directory : public Node
    {

        // BOOHOO le thing is on le heap
        std::unordered_map<std::string, Node *> files;

    public:
        int getattr(struct stat *stbuf);
        int readdir(void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
        int addentry(const std::string& name, Node *node);
        bool remove_entry(const std::string& name);
        Node* get_entry(const std::string& name);
        Directory();
        ~Directory();
    };


    class File : public Node
    {
        std::vector<unsigned char> content;
    public:
        int getattr(struct stat *stbuf);
        int read(char *buf, size_t size, off_t offset,struct fuse_file_info *fi);

        int write(const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
        int truncate(off_t size);
        File();
        ~File();
    };
}
