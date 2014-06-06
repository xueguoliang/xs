
/*
 * This file is part of the xs Library.
 *
 * Copyright (C) 2011 by Guoliang Xue <xueguoliang@gmail.com>
 *
 * The xs Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The xs Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the xs Library. If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"

int xs_file_length(const char* path)
{
#ifdef WIN32
    HANDLE hFile;
    DWORD dwFileSize = 0;


    hFile = CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,
        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        dwFileSize = GetFileSize(hFile,NULL);

        CloseHandle(hFile);
        return (int)dwFileSize;
    }

#else
    struct stat st;
    int ret = lstat(path, &st);
    if(ret < 0)
        return ret;

    if(S_ISREG(st.st_mode))
        return st.st_size;


#endif
    return -1;
}

int xs_file_exist(const char* path)
{
#ifdef WIN32
    DWORD dw;
    dw = GetFileAttributesA(path);
    if(dw != (DWORD)-1 && dw != FILE_ATTRIBUTE_DIRECTORY)
        return 1;
    return 0;

#else
    struct stat st;
    int ret = lstat(path, &st);
    if(ret < 0)
        return 0;

    if(S_ISREG(st.st_mode))
        return 1;

    return 0;
#endif
}

int xs_dir_exist(const char* path)
{
#ifdef WIN32
    DWORD dw;
    dw = GetFileAttributesA(path);
    if(dw != (DWORD)-1 && dw == FILE_ATTRIBUTE_DIRECTORY)
        return 1;
    return 0;
#else
    struct stat st;
    int ret = lstat(path, &st);
    if(ret < 0)
        return 0;

    if(S_ISDIR(st.st_mode))
        return 1;
    return 0;
#endif
}
#ifdef __cplusplus
}
#endif
