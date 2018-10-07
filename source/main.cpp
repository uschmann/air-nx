#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include <netdb.h>

#include "mongoose.h"
#include "utils.h"
#include "handler.h"
#include "cJSON.h"


static struct mg_serve_http_opts s_http_server_opts;
static const char *s_http_port = "80";
static const struct mg_str s_get_method = MG_MK_STR("GET");
static const struct mg_str s_put_method = MG_MK_STR("PUT");
static const struct mg_str s_delele_method = MG_MK_STR("DELETE");

static int has_prefix(const struct mg_str *uri, const struct mg_str *prefix) {
  return uri->len > prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0;
}

static int is_equal(const struct mg_str *s1, const struct mg_str *s2) {
  return s1->len == s2->len && memcmp(s1->p, s2->p, s2->len) == 0;
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;

  	static const struct mg_str api_prefix = MG_MK_STR("/api");
  	static const struct mg_str api_file = MG_MK_STR("/api/file");
	static const struct mg_str api_copy = MG_MK_STR("/api/file/copy");
	static const struct mg_str api_rename = MG_MK_STR("/api/file/rename");
	static const struct mg_str api_dir = MG_MK_STR("/api/directory");

	if (ev == MG_EV_HTTP_REQUEST) {
			if (has_prefix(&hm->uri, &api_prefix)) {
				// /file
				if(is_equal(&hm->uri, &api_file)) {
					if (is_equal(&hm->method, &s_get_method)) {
						handleGetFiles(nc, hm);
					}
					else if (is_equal(&hm->method, &s_delele_method)) {
						handleDeleteFile(nc, hm);
					}
				}
				// /file/copy
				if(is_equal(&hm->uri, &api_copy)) {
					handleCopy(nc, hm);
				}
				// /file/rename
				if(is_equal(&hm->uri, &api_rename)) {
					handleRename(nc, hm);
				}
				// /directory
				if(is_equal(&hm->uri, &api_dir)) {
					handleMakeDir(nc, hm);
				}
			}
			else {
				mg_serve_http(nc, hm, s_http_server_opts);
			}
	}
}

struct file_writer_data {
  FILE *fp;
  size_t bytes_written;
  char * path;
};


static void handle_upload(struct mg_connection *nc, int ev, void *p) {
  struct file_writer_data *data = (struct file_writer_data *) nc->user_data;
  struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *) p;
  struct http_message *hm = (struct http_message *) p;
  char path[PATH_MAX] = { '\0' };

  switch (ev) {
	case MG_EV_HTTP_MULTIPART_REQUEST: 
		if (data == NULL) {
			util_get_query_var(path, "path", hm);
		
			data = (struct file_writer_data *)calloc(1, sizeof(struct file_writer_data));
			data->fp = fopen(path, "w+");
			data->bytes_written = 0;
			data->path = (char*)calloc(strlen(path) + 1, sizeof(char));
			sprintf(data->path, path);

			if (data->fp == NULL) {
				mg_printf(nc, "%s",
							"HTTP/1.1 500 Failed to open a file\r\n"
							"Content-Length: 0\r\n\r\n");
				nc->flags |= MG_F_SEND_AND_CLOSE;
				free(data);
				return;
			}
			nc->user_data = (void *) data;
      	}
	break;
    case MG_EV_HTTP_PART_DATA: {
      if (fwrite(mp->data.p, 1, mp->data.len, data->fp) != mp->data.len) {
        mg_printf(nc, "%s",
                  "HTTP/1.1 500 Failed to write to a file\r\n"
                  "Content-Length: 0\r\n\r\n");
        nc->flags |= MG_F_SEND_AND_CLOSE;
        return;
      }
      data->bytes_written += mp->data.len;
      break;
    }
    case MG_EV_HTTP_PART_END: {
	  fclose(data->fp);

	  cJSON * jsonFile = util_create_json_from_file(data->path);
	  char * string = cJSON_Print(jsonFile);
	  cJSON_Delete(jsonFile);
      util_response_json(nc, string);

	  free(data->path);
	  free(data);
      nc->flags |= MG_F_SEND_AND_CLOSE;
      nc->user_data = NULL;
      break;
    }
  }
}

int main(int argc, char **argv)
{
	gfxInitDefault();

	socketInitializeDefault();
  	nxlinkStdio(); 

	struct mg_mgr mgr;
  	struct mg_connection *c;

  	mg_mgr_init(&mgr, NULL);
  	c = mg_bind(&mgr, s_http_port, ev_handler);
  	
  	s_http_server_opts.document_root = "/";
	mg_register_http_endpoint(c, "/api/upload", handle_upload);

	mg_set_protocol_http_websocket(c);

	while(appletMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

		mg_mgr_poll(&mgr, 1000);

		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();
	}
	mg_mgr_free(&mgr);

	socketExit();
	gfxExit();
	return 0;
}

