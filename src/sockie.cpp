#include <iostream>
#include <atomic>


#include <libwebsockets.h>
#include <signal.h>
#include <string.h>  // for strsignal

struct msg {
	void *payload; /* is malloc'd */
	size_t len;
};


/* one of these is created for each client connecting to us */

struct per_session_data__minimal {
	struct per_session_data__minimal *pss_list;
	struct lws *wsi;
	int last; /* the last message number we sent */
};

/* one of these is created for each vhost our protocol is used with */

struct per_vhost_data__minimal {
	struct lws_context *context;
	struct lws_vhost *vhost;
	const struct lws_protocols *protocol;

	struct per_session_data__minimal *pss_list; /* linked-list of live pss*/

	struct msg amsg; /* the one pending message... */
	int current; /* the current message number we are caching */
};




static volatile sig_atomic_t sig_caught = 0;
void signal_handler(int signum)
{
    std::cout << "Got signal: " << strsignal(signum) << std::endl;
    sig_caught = 1;
}










static int
callback_minimal(struct lws *wsi, enum lws_callback_reasons reason,
			void *user, void *in, size_t len)
{
	std::cout << __LINE__ << ": WOOOOOO" << std::endl;
    per_session_data__minimal *pss =
			(per_session_data__minimal *)user;
	per_vhost_data__minimal *vhd =
			(per_vhost_data__minimal *)
			lws_protocol_vh_priv_get(lws_get_vhost(wsi),
					lws_get_protocol(wsi));
	int m;

    switch (reason) {
    case LWS_CALLBACK_CLIENT_WRITEABLE:
        std::cout << __LINE__ << ": Client writeable!" << std::endl;
		if (!vhd->amsg.payload)
			break;

		if (pss->last == vhd->current)
			break;

		/* notice we allowed for LWS_PRE in the payload already */
		m = lws_write(wsi, ((unsigned char *)vhd->amsg.payload) +
			      LWS_PRE, vhd->amsg.len, LWS_WRITE_TEXT);
		if (m < (int)vhd->amsg.len) {
			std::cerr << "AH PISSSSSS it BROOOOKE " << std::endl;
			return -1;
		} 
        std::cout << "Wrote with some successsss!" << std::endl;
        sleep(1);
        break;
    
    case LWS_CALLBACK_SERVER_WRITEABLE:
        std::cout << __LINE__ << ": server writeable!" << std::endl;
		if (!vhd->amsg.payload)
			break;

		if (pss->last == vhd->current)
			break;

		/* notice we allowed for LWS_PRE in the payload already */
		m = lws_write(wsi, ((unsigned char *)vhd->amsg.payload) +
			      LWS_PRE, vhd->amsg.len, LWS_WRITE_TEXT);
		if (m < (int)vhd->amsg.len) {
			std::cerr << "AH PISSSSSS it BROOOOKE " << std::endl;
			return -1;
		} 
        std::cout << "Wrote with some successsss!" << std::endl;
        sleep(1);
        break;
    
    default:
        std::cerr << __LINE__ << ": I dunno what happened" << std::endl;

    }
    return 0;
}



#define LWS_PLUGIN_PROTOCOL_MINIMAL \
	{ \
		"lws-minimal", \
		callback_minimal, \
		sizeof(struct per_session_data__minimal), \
		128, \
		0, NULL, 0 \
	}


// List all acceptible protocols
// BECAUSE THE DOCS SUCK I DON'T KNOW WHAT ELSE TO PUT HERE!!!!
static struct lws_protocols protocols[] = {
	{ "http", lws_callback_http_dummy, 0, 0 },
	LWS_PLUGIN_PROTOCOL_MINIMAL,
	{ NULL, NULL, 0, 0 } /* terminator */
};


static const struct lws_http_mount mount = {
	/* .mount_next */		NULL,		/* linked-list "next" */
	/* .mountpoint */		"/derp",		/* mountpoint URL */
	/* .origin */			NULL,  /* serve from dir */
	/* .def */			NULL,	/* default filename */
	/* .protocol */			NULL,
	/* .cgienv */			NULL,
	/* .extra_mimetypes */		NULL,
	/* .interpret */		NULL,
	/* .cgi_timeout */		0,
	/* .cache_max_age */		0,
	/* .auth_mask */		0,
	/* .cache_reusable */		0,
	/* .cache_revalidate */		0,
	/* .cache_intermediaries */	0,
	/* .origin_protocol */		LWSMPRO_FILE,	/* files in a dir */
	/* .mountpoint_len */		1,		/* char count */
	/* .basic_auth_login_file */	NULL,
};



int do_the_websockie_thang(unsigned int port)
{
    int flag = 0;
    struct lws_context_creation_info info;
	struct lws_context *context;

    memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
	info.port = port;
	info.mounts = &mount;
	info.protocols = protocols;
	info.vhost_name = "localhost";
	info.ws_ping_pong_interval = 10;
	info.options =
		LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

    context = lws_create_context(&info);
	if (!context) {
		lwsl_err("lws init failed\n");
		return 1;
	}

    unsigned int count(0);
    while (flag >= 0 && 0 == sig_caught) {
		flag = lws_service(context, 1000);
        std::cout << "Count: " << count++ << std::endl;
    }

    lws_context_destroy(context);
    return 0;
}



int main() {
    signal(SIGINT, signal_handler);

    std::cout << "oh BARF" << std::endl;
    do_the_websockie_thang(9998);
    return 0;
}