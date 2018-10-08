#pragma once

#include "mongoose.h"
#include "cJSON.h"

void util_copy_mg_str(char * dest, mg_str* src);
bool util_get_query_var(char* dest, char* name, struct http_message * request);
void util_response_ok(struct mg_connection *nc, char* content);
void util_response_404(struct mg_connection *nc, char* content);
void util_response_409(struct mg_connection *nc, char* content);
void util_response_json(struct mg_connection *nc, char* content);
char *utils_Basename(const char *filename);
void rmtree(const char path[]);
cJSON * util_create_json_from_file(char * path);
int util_decode(const char *s, char *dec);