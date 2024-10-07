#include "filesystem.h"
#include "utils.h"
#include <iostream>
using namespace FS;





int File::read(char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
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
        content[dst] = buf[i];
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

int File::truncate(off_t size) {
    content.resize(size);
    return size;
}

File::File() {
    type = FiletypeEnum::FILE;
}