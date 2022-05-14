MYSQLND_METHOD(mysqlnd_conn_data, set_server_option)(MYSQLND_CONN_DATA * const conn, enum_mysqlnd_server_option option TSRMLS_DC)
{
	size_t this_func = STRUCT_OFFSET(struct st_mysqlnd_conn_data_methods, set_server_option);
	zend_uchar buffer[2];
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_conn_data::set_server_option");
	if (PASS == conn->m->local_tx_start(conn, this_func TSRMLS_CC)) {
 
                int2store(buffer, (unsigned int) option);
                ret = conn->m->simple_command(conn, COM_SET_OPTION, buffer, sizeof(buffer), PROT_EOF_PACKET, FALSE, TRUE TSRMLS_CC);
                conn->m->local_tx_end(conn, this_func, ret TSRMLS_CC);
        }
        DBG_RETURN(ret);
}
