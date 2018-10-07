#include <string.h>
#include <stdio.h>

#include <switch.h>

#include <netdb.h>
#include "mongoose.h"
#include "utils.h"
#include "handler.h"

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
			}
			else {
				mg_serve_http(nc, hm, s_http_server_opts);
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
  	mg_set_protocol_http_websocket(c);

  	s_http_server_opts.document_root = "/";

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

