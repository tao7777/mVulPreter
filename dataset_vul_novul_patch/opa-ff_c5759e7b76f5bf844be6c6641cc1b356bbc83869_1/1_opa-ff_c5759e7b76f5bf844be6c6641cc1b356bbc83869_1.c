hcom_client_init
(
		OUT	p_hsm_com_client_hdl_t	*p_hdl,
	IN		char					*server_path,
	IN		char					*client_path,
	IN		int						max_data_len
)
{
	hsm_com_client_hdl_t	*hdl = NULL;
	hsm_com_errno_t			res = HSM_COM_OK;
	

	if((strlen(server_path) > (HSM_COM_SVR_MAX_PATH - 1)) ||
	   (strlen(server_path) == 0)){
		res = HSM_COM_PATH_ERR;
		goto cleanup;
	}

	if((strlen(client_path) > (HSM_COM_SVR_MAX_PATH - 1)) ||
	   (strlen(client_path) == 0)){
		res = HSM_COM_PATH_ERR;
		goto cleanup;
	}


	if((hdl = calloc(1,sizeof(hsm_com_client_hdl_t))) == NULL)
	{
		res = HSM_COM_NO_MEM;
		goto cleanup;
	}

	if((hdl->scr.scratch = malloc(max_data_len)) == NULL) 
	{
		res = HSM_COM_NO_MEM;
		goto cleanup;
	}

	if((hdl->recv_buf = malloc(max_data_len)) == NULL) 
	{
		res = HSM_COM_NO_MEM;
		goto cleanup;
	}

	if((hdl->send_buf = malloc(max_data_len)) == NULL) 
	{
		res = HSM_COM_NO_MEM;
		goto cleanup;
	}

	hdl->scr.scratch_fill = 0;
	hdl->scr.scratch_len = max_data_len;
	hdl->buf_len = max_data_len;
	hdl->trans_id = 1;


 	strcpy(hdl->s_path,server_path);
 	strcpy(hdl->c_path,client_path);
 
 
 	hdl->client_state = HSM_COM_C_STATE_IN;
 
	*p_hdl = hdl;

	return res;

cleanup:
	if(hdl)
	{
		if (hdl->scr.scratch) {
			free(hdl->scr.scratch);
		}
		if (hdl->recv_buf) {
			free(hdl->recv_buf);
		}
		free(hdl);
	}

	return res;

}
