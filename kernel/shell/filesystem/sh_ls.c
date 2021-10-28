#include "sh_ls.h"

#include "../../fs/fs.h"

int32_t sh_ls(int argc, char* argv[])
{
    fs_list_files();
    return 0;
}