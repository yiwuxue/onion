/** Licensed under AGPL 3.0. (C) 2010 David Moreno Montero. http://coralbits.com */
#include <onion/onion.h>
#include <onion/log.h>
#include <signal.h>
#include <netdb.h>

int hello(void *p, onion_request *req, onion_response *res){
	//onion_response_set_length(res, 11);
	onion_response_write0(res,"Hello world");
	if (onion_request_get_query(req, "1")){
		onion_response_printf(res, "<p>Path: %s", onion_request_get_query(req, "1"));
	}
	onion_response_printf(res,"<p>Client description: %s",onion_request_get_client_description(req));
  
  // Now using sockaddr_storage
  struct sockaddr_storage *client_addr;
  socklen_t client_len;
  client_addr=onion_request_get_sockadd_storage(req, &client_len);
  
  char tmp[256];
  getnameinfo((struct sockaddr *)client_addr, client_len, tmp, sizeof(tmp),
    NULL, 0, NI_NUMERICHOST);
  
  onion_response_printf(res,"<p>Client description bis: %s", tmp);
  tmp[0]=0;
  getnameinfo((struct sockaddr *)client_addr, client_len, tmp, sizeof(tmp),
    NULL, 0, NI_NAMEREQD);
  onion_response_printf(res,"<p>Client name: %s", tmp);
  
	return OCS_PROCESSED;
}

onion *o=NULL;

static void shutdown_server(int _){
	if (o) 
		onion_listen_stop(o);
}

int main(int argc, char **argv){
	signal(SIGINT,shutdown_server);
	signal(SIGTERM,shutdown_server);
	
	o=onion_new(O_POOL);
	onion_set_timeout(o, 5000);
	//onion_set_hostname(o,"0.0.0.0");
	onion_url *urls=onion_root_url(o);
	
	onion_url_add_static(urls, "static", "Hello static world", HTTP_OK);
	onion_url_add(urls, "", hello);
	onion_url_add(urls, "^(.*)$", hello);
	
	onion_listen(o);
	onion_free(o);
	return 0;
}
