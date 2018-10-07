#include "handler.h"
#include "cJson.h"
#include "utils.h"
#include <dirent.h>
#include "fs.h"
#include <limits.h> 
#include <stdlib.h>

void handleGetFiles(struct mg_connection *nc, http_message *hm)
{
    char dir[1024] = { '\0' };
	if(util_get_query_var(dir, "dir", hm)) {
        printf("%s\n", dir);
    }
    else {
        sprintf(dir, "/");
    }

    DIR* directory;
	struct dirent* ent;

    directory = opendir(dir);
	if(directory == NULL)
	{
		printf("Failed to open dir.\n");
        mg_printf(nc, "%s", "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
	}
    else {
        cJSON *array = cJSON_CreateArray();
        cJSON *file  = NULL;
		char fullPath[PATH_MAX + 1]; 

        while ((ent = readdir(directory)))
		{
            file = cJSON_CreateObject();
            if(dir[strlen(dir) -1] == '/') {
                sprintf(fullPath, "%s%s", dir, ent->d_name);
            }
            else {
                sprintf(fullPath, "%s/%s", dir, ent->d_name);
            }
            

            // name
            cJSON *name = cJSON_CreateString(ent->d_name);
            cJSON_AddItemToObject(file, "name", name);
            cJSON *path = cJSON_CreateString(fullPath);
            cJSON_AddItemToObject(file, "path", path);
            // ext
            cJSON *ext = cJSON_CreateString(FS_GetFileExt(ent->d_name));
            cJSON_AddItemToObject(file, "extension", ext);
            // last_modified
            cJSON *lastModified = cJSON_CreateString(FS_GetFileModifiedTime(fullPath));
            cJSON_AddItemToObject(file, "last_modified", lastModified);
            // is_dir
            cJSON *isDir = FS_IsDirectory(fullPath) == 1 ? cJSON_CreateTrue() : cJSON_CreateFalse();
            cJSON_AddItemToObject(file, "is_directory", isDir);

            cJSON_AddItemToArray(array, file);
		}
		closedir(directory);

        char* string = cJSON_Print(array);
        cJSON_Delete(array);
        
        util_response_json(nc, string);
    }
}