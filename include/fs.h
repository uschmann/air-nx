#pragma once

#include <switch.h>

int FS_MakeDir(const char *path);
int FS_RecursiveMakeDir(const char * dir);
bool FS_FileExists(const char *path);
bool FS_DirExists(const char *path);
const char *FS_GetFileExt(const char *filename);
char *FS_GetFileModifiedTime(const char *filename);
u64 FS_GetFileSize(const char *filename);
int FS_IsDirectory(const char *path);