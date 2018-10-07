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
            if(dir[strlen(dir) -1] == '/') {
                sprintf(fullPath, "%s%s", dir, ent->d_name);
            }
            else {
                sprintf(fullPath, "%s/%s", dir, ent->d_name);
            }
            file = util_create_json_from_file(fullPath);

            cJSON_AddItemToArray(array, file);
		}
		closedir(directory);

        char* string = cJSON_Print(array);
        cJSON_Delete(array);
        
        util_response_json(nc, string);
    }
}

void handleDeleteFile(struct mg_connection *nc, http_message *hm)
{
    char path[1024] = { '\0' };
	if(util_get_query_var(path, "path", hm)) {
        if(FS_FileExists(path) || FS_DirExists(path)) {
            if(FS_IsDirectory(path)) {
                rmtree(path);
            }
            else {
                remove(path);   
            }

            cJSON* file = util_create_json_from_file(path);
            char* string = cJSON_Print(file);
            cJSON_Delete(file);

            util_response_json(nc, string);
        }
        else {
            util_response_404(nc, "Not Found");
        }
    }
}

void handleMakeDir(struct mg_connection *nc, http_message *hm)
{
    char path[1024] = { '\0' };
	if(util_get_query_var(path, "path", hm)) {
        if(FS_FileExists(path) || FS_DirExists(path)) {
            util_response_409(nc, "File already exists");
        }
        else {
            FS_RecursiveMakeDir(path);

            cJSON* file = util_create_json_from_file(path);
            char* string = cJSON_Print(file);
            cJSON_Delete(file);

            util_response_json(nc, string);
        }
    }
}

void handleCopy(struct mg_connection *nc, http_message *hm)
{
    char from[FS_MAX_PATH] = { '\0' };
    char to[FS_MAX_PATH] = { '\0' };
    util_get_query_var(from, "from", hm);
    util_get_query_var(to, "to", hm);

    if(!FS_FileExists(from) && !FS_DirExists(from)) {
        util_response_404(nc, "Not found from");
    }
    else {
        if(FS_IsDirectory(from)) {
            FS_CopyDir(from, to);
        }
        else {
            FS_CopyFile(from, to);
        }

        cJSON * jsonFile = util_create_json_from_file(to);
	    char * string = cJSON_Print(jsonFile);
	    cJSON_Delete(jsonFile);
        util_response_json(nc, string);
    }
}

void handleRename(struct mg_connection *nc, http_message *hm)
{
    char from[FS_MAX_PATH] = { '\0' };
    char to[FS_MAX_PATH] = { '\0' };
    util_get_query_var(from, "from", hm);
    util_get_query_var(to, "to", hm);

    printf("Rename: %s -> %s\n", from, to);

    if(!FS_FileExists(from) && !FS_DirExists(from)) {
        util_response_404(nc, "Not found from");
    }
    else {
        rename(from, to);

        cJSON * jsonFile = util_create_json_from_file(to);
	    char * string = cJSON_Print(jsonFile);
	    cJSON_Delete(jsonFile);
        util_response_json(nc, string);
    }
}