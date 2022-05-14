mysqlnd_switch_to_ssl_if_needed(
			MYSQLND_CONN_DATA * conn,
			const MYSQLND_PACKET_GREET * const greet_packet,
			const MYSQLND_OPTIONS * const options,
			unsigned long mysql_flags
			TSRMLS_DC
		)
{
	enum_func_status ret = FAIL;
	const MYSQLND_CHARSET * charset;
	MYSQLND_PACKET_AUTH * auth_packet;
	DBG_ENTER("mysqlnd_switch_to_ssl_if_needed");

	auth_packet = conn->protocol->m.get_auth_packet(conn->protocol, FALSE TSRMLS_CC);
	if (!auth_packet) {
		SET_OOM_ERROR(*conn->error_info);
		goto end;
	}
	auth_packet->client_flags = mysql_flags;
	auth_packet->max_packet_size = MYSQLND_ASSEMBLED_PACKET_MAX_SIZE;

	if (options->charset_name && (charset = mysqlnd_find_charset_name(options->charset_name))) {
		auth_packet->charset_no	= charset->nr;
	} else {
#if MYSQLND_UNICODE
		auth_packet->charset_no	= 200;/* utf8 - swedish collation, check mysqlnd_charset.c */
#else
		auth_packet->charset_no	= greet_packet->charset_no;
#endif
        }
 
 #ifdef MYSQLND_SSL_SUPPORTED
       if (mysql_flags & CLIENT_SSL) {
               zend_bool server_has_ssl = (greet_packet->server_capabilities & CLIENT_SSL)? TRUE:FALSE;
               if (server_has_ssl == FALSE) {
                       goto close_conn;
               } else {
                       zend_bool verify = mysql_flags & CLIENT_SSL_VERIFY_SERVER_CERT? TRUE:FALSE;
                       DBG_INF("Switching to SSL");
                       if (!PACKET_WRITE(auth_packet, conn)) {
                               goto close_conn;
                       }
 
                       conn->net->m.set_client_option(conn->net, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, (const char *) &verify TSRMLS_CC);
 
                       if (FAIL == conn->net->m.enable_ssl(conn->net TSRMLS_CC)) {
                               goto end;
                       }
                }
        }
#else
       auth_packet->client_flags &= ~CLIENT_SSL;
       if (!PACKET_WRITE(auth_packet, conn)) {
               goto close_conn;
       }
 #endif
        ret = PASS;
 end:
        PACKET_FREE(auth_packet);
        DBG_RETURN(ret);

close_conn:
       CONN_SET_STATE(conn, CONN_QUIT_SENT);
       conn->m->send_close(conn TSRMLS_CC);
       SET_CLIENT_ERROR(*conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
       PACKET_FREE(auth_packet);
       DBG_RETURN(ret);
 }
