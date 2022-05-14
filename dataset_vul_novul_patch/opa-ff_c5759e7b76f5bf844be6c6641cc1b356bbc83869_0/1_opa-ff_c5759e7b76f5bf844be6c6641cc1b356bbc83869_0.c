unix_client_connect(hsm_com_client_hdl_t *hdl)
 {
 	int					fd, len;
 	struct sockaddr_un	unix_addr;
 
 	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) 
 	{
		return HSM_COM_ERROR;
	}

	memset(&unix_addr,0,sizeof(unix_addr));

	unix_addr.sun_family = AF_UNIX;
 	
 	if(strlen(hdl->c_path) >= sizeof(unix_addr.sun_path))
 	{
		close(fd);
		return HSM_COM_PATH_ERR;
 	}
 
 	snprintf(unix_addr.sun_path, sizeof(unix_addr.sun_path), "%s", hdl->c_path);

	len = SUN_LEN(&unix_addr);

	unlink(unix_addr.sun_path);
 
 	if(bind(fd, (struct sockaddr *)&unix_addr, len) < 0)
 	{
		unlink(hdl->c_path);
		close(fd);
		return HSM_COM_BIND_ERR;
 	}
 
 	if(chmod(unix_addr.sun_path, S_IRWXU) < 0)
 	{
		unlink(hdl->c_path);
		close(fd);
		return HSM_COM_CHMOD_ERR;
 	}
 
 	memset(&unix_addr,0,sizeof(unix_addr));

	unix_addr.sun_family = AF_UNIX;
	strncpy(unix_addr.sun_path, hdl->s_path, sizeof(unix_addr.sun_path));
	unix_addr.sun_path[sizeof(unix_addr.sun_path)-1] = 0;

	len = SUN_LEN(&unix_addr);
 
 	if (connect(fd, (struct sockaddr *) &unix_addr, len) < 0) 
 	{
		unlink(hdl->c_path);
		close(fd);
		return HSM_COM_CONX_ERR;
 	}
 
 	hdl->client_fd = fd;
	hdl->client_state = HSM_COM_C_STATE_CT;

 	if(unix_sck_send_conn(hdl, 2) != HSM_COM_OK)
 	{
 		hdl->client_state = HSM_COM_C_STATE_IN;
		return HSM_COM_SEND_ERR;
 	}
 
 
	return HSM_COM_OK;
 
 }
