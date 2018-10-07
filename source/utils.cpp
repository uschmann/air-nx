#include "utils.h"
#include "yuarel.h"

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
            strcpy(dest, params[i].val);
            return true;
        }
    }
    return false;
}

void util_response_ok(struct mg_connection *nc, char* content)
{
    mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s", strlen(content), content);
}

void util_response_json(struct mg_connection *nc, char* content)
{
    mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: application/json\r\n\r\n%s", strlen(content), content);
}