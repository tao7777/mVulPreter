void close_connection(h2o_http2_conn_t *conn)
int close_connection(h2o_http2_conn_t *conn)
 {
     conn->state = H2O_HTTP2_CONN_STATE_IS_CLOSING;
 
     if (conn->_write.buf_in_flight != NULL || h2o_timeout_is_linked(&conn->_write.timeout_entry)) {
         /* there is a pending write, let on_write_complete actually close the connection */
     } else {
         close_connection_now(conn);
        return -1;
     }
    return 0;
 }
