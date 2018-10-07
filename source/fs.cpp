#include "fs.h"

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

int FS_MakeDir(const char *path)
{
	if (!path) 
		return -1;

	return mkdir(path, 0777);
}

int FS_RecursiveMakeDir(const char * dir)
{
	int ret = 0;
	char buf[256];
	char *p = NULL;
	size_t len;

	snprintf(buf, sizeof(buf), "%s",dir);
	len = strlen(buf);

	if (buf[len - 1] == '/')
		buf[len - 1] = 0;

	for (p = buf + 1; *p; p++)
	{
		if (*p == '/') 
		{
			*p = 0;

			ret = FS_MakeDir(buf);
			
			*p = '/';
		}
		
		ret = FS_MakeDir(buf);
	}
	
	return ret;
}

bool FS_FileExists(const char *path)
{
	FILE *temp = fopen(path, "r");
	
	if (temp == NULL)
		return false;
	
	fclose(temp);
	return true;
}

bool FS_DirExists(const char *path)
{
	struct stat info;

	if (stat(path, &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;
	else
		return false;
}

const char *FS_GetFileExt(const char *filename) 
{
	const char *dot = strrchr(filename, '.');
	
	if (!dot || dot == filename)
		return "";
	
	return dot + 1;
}

char *FS_GetFileModifiedTime(const char *filename) 
{
	struct stat attr;
	stat(filename, &attr);
	
	return ctime(&attr.st_mtime);
}

u64 FS_GetFileSize(const char *filename)
{
	struct stat st;
	stat(filename, &st);
	
	return st.st_size;
}

int FS_IsDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

// Copy file from src to dst
int FS_CopyFile(char *src, char *dst)
{
	int chunksize = (512 * 1024); // Chunk size
	char *buffer = (char *)malloc(chunksize); // Reading buffer

	u64 totalwrite = 0; // Accumulated writing
	u64 totalread = 0; // Accumulated reading

	int result = 0; // Result

	int in = open(src, O_RDONLY, 0777); // Open file for reading
	u64 size = FS_GetFileSize(src);

	// Opened file for reading
	if (in >= 0)
	{
		if (FS_FileExists(dst))
			remove(dst); // Delete output file (if existing)

		int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0777); // Open output file for writing

		if (out >= 0) // Opened file for writing
		{
			u64 b_read = 0; // Read byte count

			// Copy loop (512KB at a time)
			while((b_read = read(in, buffer, chunksize)) > 0)
			{
				totalread += b_read; // Accumulate read data
				totalwrite += write(out, buffer, b_read); // Write data
			}

			close(out); // Close output file
			
			if (totalread != totalwrite) // Insufficient copy
				result = -3;
		}
		
		else // Output open error
			result = -2;
			
		close(in); // Close input file
	}

	// Input open error
	else
		result = -1;
	
	free(buffer); // Free memory
	return result; // Return result
}

Result FS_CopyDir(char *src, char *dst)
{
	DIR *directory = opendir(src);

	if (directory)
	{
		// Create Output Directory (is allowed to fail, we can merge folders after all)
		FS_MakeDir(dst);
		
		struct dirent *entries;

		// Iterate Files
		while ((entries = readdir(directory)) != NULL)
		{
			if (strlen(entries->d_name) > 0)
			{
				// Calculate Buffer Size
				int insize = strlen(src) + strlen(entries->d_name) + 2;
				int outsize = strlen(dst) + strlen(entries->d_name) + 2;

				// Allocate Buffer
				char *inbuffer = (char *)malloc(insize);
				char *outbuffer = (char *)malloc(outsize);

				// Puzzle Input Path
				strcpy(inbuffer, src);
				inbuffer[strlen(inbuffer) + 1] = 0;
				inbuffer[strlen(inbuffer)] = '/';
				strcpy(inbuffer + strlen(inbuffer), entries->d_name);

				// Puzzle Output Path
				strcpy(outbuffer, dst);
				outbuffer[strlen(outbuffer) + 1] = 0;
				outbuffer[strlen(outbuffer)] = '/';
				strcpy(outbuffer + strlen(outbuffer), entries->d_name);

				// Another Folder
				if (entries->d_type == DT_DIR)
					FS_CopyDir(inbuffer, outbuffer); // Copy Folder (via recursion)

				// Simple File
				else
					FS_CopyFile(inbuffer, outbuffer); // Copy File

				// Free Buffer
				free(inbuffer);
				free(outbuffer);
			}
		}

		closedir(directory);
		return 0;
	}
    else {
        printf("Could not open %s\n", src);
    }

	return -1;
}