#include "utils.h"
#include "yuarel.h"
#include <string.h>
#include <stdio.h>
#include "fs.h"

void util_copy_mg_str(char * dest, mg_str* src)
{
    dest[src->len] = '\0';
	memcpy(dest, src->p, src->len);
}

bool util_get_query_var(char* dest, char* name, struct http_message * request)
{
    char query[1024];
	util_copy_mg_str(query, &request->query_string);

    struct yuarel_param params[10];
	int numberOfParams = yuarel_parse_query(query, '&', params, 10);

    for(int i = 0; i < numberOfParams; i++) {
        if(strcmp(params[i].key, name) == 0) {
            util_decode(params[i].val, dest);
            return true;
        }
    }
    return false;
}

void util_response_ok(struct mg_connection *nc, char* content)
{
    mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s", strlen(content), content);
}

void util_response_404(struct mg_connection *nc, char* content)
{
    mg_printf(nc, "HTTP/1.1 404 Not Found\r\nContent-Length: %d\r\n\r\n%s", strlen(content), content);
}

void util_response_409(struct mg_connection *nc, char* content)
{
    mg_printf(nc, "HTTP/1.1 409 Conflict\r\nContent-Length: %d\r\n\r\n%s", strlen(content), content);
}

void util_response_json(struct mg_connection *nc, char* content)
{
    mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: application/json\r\n\r\n%s", strlen(content), content);
}

char *utils_Basename(const char *filename)
{
	char *p = strrchr (filename, '/');
	return p ? p + 1 : (char *) filename;
}

void rmtree(const char path[])
{
    size_t path_len;
    char *full_path;
    DIR *dir;
    struct stat stat_path, stat_entry;
    struct dirent *entry;

    // stat for the path
    stat(path, &stat_path);

    // if path does not exists or is not dir - exit with status -1
    if (S_ISDIR(stat_path.st_mode) == 0) {
        fprintf(stderr, "%s: %s\n", "Is not directory", path);
    }

    // if not possible to read the directory for this user
    if ((dir = opendir(path)) == NULL) {
        fprintf(stderr, "%s: %s\n", "Can`t open directory", path);
    }

    // the length of the path
    path_len = strlen(path);

    // iteration through entries in the directory
    while ((entry = readdir(dir)) != NULL) {

        // skip entries "." and ".."
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        // determinate a full path of an entry
        full_path = (char*)calloc(path_len + strlen(entry->d_name) + 1, sizeof(char));
        strcpy(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, entry->d_name);

        // stat for the entry
        stat(full_path, &stat_entry);

        // recursively remove a nested directory
        if (S_ISDIR(stat_entry.st_mode) != 0) {
            rmtree(full_path);
            continue;
        }

        // remove a file object
        if (remove(full_path) == 0)
            printf("Removed a file: %s\n", full_path);
        else
            printf("Can`t remove a file: %s\n", full_path);
    }

    // remove the devastated directory and close the object of it
    closedir(dir);

    if (rmdir(path) == 0)
        printf("Removed a directory: %s\n", path);
    else
        printf("Can`t remove a directory: %s Error: %d\n", path, rmdir(path));
}

cJSON * util_create_json_from_file(char * path)
{
    cJSON* file = cJSON_CreateObject();

    // name
    cJSON *name = cJSON_CreateString(utils_Basename(path));
    cJSON_AddItemToObject(file, "name", name);
    cJSON *pathName = cJSON_CreateString(path);
    cJSON_AddItemToObject(file, "path", pathName);
    // ext
    cJSON *ext = cJSON_CreateString(FS_GetFileExt(path));
    cJSON_AddItemToObject(file, "extension", ext);
    // last_modified
    cJSON *lastModified = cJSON_CreateString(FS_GetFileModifiedTime(path));
    cJSON_AddItemToObject(file, "last_modified", lastModified);
    // is_dir
    cJSON *isDir = FS_IsDirectory(path) == 1 ? cJSON_CreateTrue() : cJSON_CreateFalse();
    cJSON_AddItemToObject(file, "is_directory", isDir);

    return file;
}

int ishex(int x)
{
	return	(x >= '0' && x <= '9')	||
		(x >= 'a' && x <= 'f')	||
		(x >= 'A' && x <= 'F');
}

int util_decode(const char *s, char *dec)
{
	char *o;
	const char *end = s + strlen(s);
	int c;
 
	for (o = dec; s <= end; o++) {
		c = *s++;
		if (c == '+') c = ' ';
		else if (c == '%' && (	!ishex(*s++)	||
					!ishex(*s++)	||
					!sscanf(s - 2, "%2x", &c)))
			return -1;
 
		if (dec) *o = c;
	}
 
	return o - dec;
}