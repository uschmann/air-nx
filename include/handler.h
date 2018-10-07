#pragma once

#include "mongoose.h"
#include "utils.h"

void handleGetFiles(struct mg_connection *nc, http_message *hm);
void handleDeleteFile(struct mg_connection *nc, http_message *hm);
void handleMakeDir(struct mg_connection *nc, http_message *hm);
void handleCopy(struct mg_connection *nc, http_message *hm);