 static void on_read(h2o_socket_t *sock, int status)
{
    h2o_http2_conn_t *conn = sock->data;

    if (status != 0) {
        h2o_socket_read_stop(conn->sock);
        close_connection(conn);
        return;
     }
 
     update_idle_timeout(conn);
    if (parse_input(conn) != 0)
        return;
 
     /* write immediately, if there is no write in flight and if pending write exists */
     if (h2o_timeout_is_linked(&conn->_write.timeout_entry)) {
        h2o_timeout_unlink(&conn->_write.timeout_entry);
        do_emit_writereq(conn);
    }
}
