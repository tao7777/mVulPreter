GIOChannel *net_connect_ip_ssl(IPADDR *ip, int port, IPADDR *my_ip, const char *cert, const char *pkey, const char *cafile, const char *capath, gboolean verify)
GIOChannel *net_connect_ip_ssl(IPADDR *ip, int port, const char* hostname, IPADDR *my_ip, const char *cert, const char *pkey, const char *cafile, const char *capath, gboolean verify)
 {
 	GIOChannel *handle, *ssl_handle;
 
 	handle = net_connect_ip(ip, port, my_ip);
 	if (handle == NULL)
 		return NULL;
	ssl_handle  = irssi_ssl_get_iochannel(handle, hostname, cert, pkey, cafile, capath, verify);
 	if (ssl_handle == NULL)
 		g_io_channel_unref(handle);
 	return ssl_handle;
}
