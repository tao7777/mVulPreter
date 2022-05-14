fm_mgr_config_mgr_connect
(
	fm_config_conx_hdl	*hdl, 
	fm_mgr_type_t 		mgr
)
{
	char                    s_path[256];
 	char                    c_path[256];
 	char                    *mgr_prefix;
 	p_hsm_com_client_hdl_t  *mgr_hdl;
	pid_t                   pid;
 
 	memset(s_path,0,sizeof(s_path));
 	memset(c_path,0,sizeof(c_path));
 
	pid = getpid();
 	switch ( mgr )
 	{
 		case FM_MGR_SM:
			mgr_prefix  = HSM_FM_SCK_SM;
			mgr_hdl     = &hdl->sm_hdl;
			break;
		case FM_MGR_PM:
			mgr_prefix  = HSM_FM_SCK_PM;
			mgr_hdl     = &hdl->pm_hdl;
			break;
		case FM_MGR_FE:
			mgr_prefix  = HSM_FM_SCK_FE;
			mgr_hdl     = &hdl->fe_hdl;
			break;
		default:
			return FM_CONF_INIT_ERR;
	}

 	sprintf(s_path,"%s%s%d",HSM_FM_SCK_PREFIX,mgr_prefix,hdl->instance);
 
	sprintf(c_path,"%s%s%d_C_%lu",HSM_FM_SCK_PREFIX,mgr_prefix,
			hdl->instance, (long unsigned)pid);
 
 	if ( *mgr_hdl == NULL )
 	{
		if ( hcom_client_init(mgr_hdl,s_path,c_path,32768) != HSM_COM_OK )
		{
			return FM_CONF_INIT_ERR;
		}
	}

	if ( hcom_client_connect(*mgr_hdl) == HSM_COM_OK )
	{
		hdl->conx_mask |= mgr;
		return FM_CONF_OK;
	}

	return FM_CONF_CONX_ERR;

}
