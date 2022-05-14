fm_mgr_config_init
(
					OUT	p_fm_config_conx_hdlt		*p_hdl,
				IN		int							instance,
	OPTIONAL	IN		char						*rem_address,
	OPTIONAL	IN		char						*community
)
{
	fm_config_conx_hdl      *hdl;
	fm_mgr_config_errno_t   res = FM_CONF_OK;


	if ( (hdl = calloc(1,sizeof(fm_config_conx_hdl))) == NULL )
	{
		res = FM_CONF_NO_MEM;
		goto cleanup;
	}

	hdl->instance = instance;

	*p_hdl = hdl;

	if(!rem_address || (strcmp(rem_address,"localhost") == 0))
	{
		if ( fm_mgr_config_mgr_connect(hdl, FM_MGR_SM) == FM_CONF_INIT_ERR )
		{
			res = FM_CONF_INIT_ERR;
			goto cleanup;
		}

		if ( fm_mgr_config_mgr_connect(hdl, FM_MGR_PM) == FM_CONF_INIT_ERR )
		{
			res = FM_CONF_INIT_ERR;
			goto cleanup;
		}

		if ( fm_mgr_config_mgr_connect(hdl, FM_MGR_FE) == FM_CONF_INIT_ERR )
		{
			res = FM_CONF_INIT_ERR;
			goto cleanup;
 		}
 	}
 
	return res;
	cleanup:
	if ( hdl ) {
		free(hdl);
		hdl = NULL;
	}
 	return res;
 }
