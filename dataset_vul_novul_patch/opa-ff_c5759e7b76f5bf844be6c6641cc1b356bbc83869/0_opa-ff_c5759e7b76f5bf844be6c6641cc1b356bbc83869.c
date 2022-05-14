 int main(int argc, char *argv[]) {
	p_fm_config_conx_hdlt	hdl = NULL;
 	int						instance = 0;
 	fm_mgr_config_errno_t	res;
 	char					*rem_addr = NULL;
	char					*community = "public";
	char            		Opts[256];
    int             		arg;
	char 					*command;
	int						i;

	/* Get options at the command line (overide default values) */
    strcpy(Opts, "i:d:h-");

    while ((arg = getopt(argc, argv, Opts)) != EOF) {
        switch (arg) {
		case 'h':
		case '-':
			usage(argv[0]);
			return(0);
		case 'i':
			instance = atol(optarg);
			break;
		case 'd':
			rem_addr = optarg;
			break;
		default:
			usage(argv[0]);
			return(-1);
		}
	}

	if(optind >= argc){
        fprintf(stderr, "Command required\n");
		usage(argv[0]);
		return -1;
	}

	command = argv[optind++];
	printf("Connecting to %s FM instance %d\n", (rem_addr==NULL) ? "LOCAL":rem_addr, instance);
 	if((res = fm_mgr_config_init(&hdl,instance, rem_addr, community)) != FM_CONF_OK)
 	{
 		fprintf(stderr, "Failed to initialize the client handle: %d\n", res);
		goto cleanup;
 	}
 
 	if((res = fm_mgr_config_connect(hdl)) != FM_CONF_OK)
 	{
 		fprintf(stderr, "Failed to connect: (%d) %s\n",res,fm_mgr_get_error_str(res));
		goto cleanup;
 	}
 
 	for(i=0;i<commandListLen;i++){
 		if(strcmp(command,commandList[i].name) == 0){
			res = commandList[i].cmdPtr(hdl, commandList[i].mgr, (argc - optind), &argv[optind]);
			goto cleanup;
 		}
 	}
 
 	fprintf(stderr, "Command (%s) is not valid\n",command);
 	usage(argv[0]);
 	res = -1;
 
cleanup:
	if (hdl)
	{
		if (hdl->sm_hdl)
		{
			if (hdl->sm_hdl->c_path[0])
				unlink(hdl->sm_hdl->c_path);
		}
		if (hdl->pm_hdl)
		{
			if (hdl->pm_hdl->c_path[0])
				unlink(hdl->pm_hdl->c_path);
		}
		if (hdl->fe_hdl)
		{
			if (hdl->fe_hdl->c_path[0])
				unlink(hdl->fe_hdl->c_path);
		}
		free(hdl);
	}

 	return res;
 }
