#pragma once

#include "mongoose.h"
#include "utils.h"

void handleGetFiles(struct mg_connection *nc, http_message *hm);
void handleDeleteFile(struct mg_connection *nc, http_message *hm);