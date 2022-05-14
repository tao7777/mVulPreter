int config__parse_args(struct mosquitto_db *db, struct mosquitto__config *config, int argc, char *argv[])
{
	int i;
	int port_tmp;

	for(i=1; i<argc; i++){
		if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")){
			if(i<argc-1){
				db->config_file = argv[i+1];

				if(config__read(db, config, false)){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open configuration file.");
					return MOSQ_ERR_INVAL;
				}
			}else{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: -c argument given, but no config file specified.");
				return MOSQ_ERR_INVAL;
			}
			i++;
		}else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--daemon")){
			config->daemon = true;
		}else if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
			print_usage();
			return MOSQ_ERR_INVAL;
		}else if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){
			if(i<argc-1){
				port_tmp = atoi(argv[i+1]);
				if(port_tmp<1 || port_tmp>65535){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Invalid port specified (%d).", port_tmp);
					return MOSQ_ERR_INVAL;
				}else{
					if(config->default_listener.port){
						log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Default listener port specified multiple times. Only the latest will be used.");
					}
					config->default_listener.port = port_tmp;
				}
			}else{
				log__printf(NULL, MOSQ_LOG_ERR, "Error: -p argument given, but no port specified.");
				return MOSQ_ERR_INVAL;
			}
			i++;
		}else if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){
			db->verbose = true;
		}else{
			fprintf(stderr, "Error: Unknown option '%s'.\n",argv[i]);
			print_usage();
			return MOSQ_ERR_INVAL;
		}
	}

	if(config->listener_count == 0
#ifdef WITH_TLS
			|| config->default_listener.cafile
			|| config->default_listener.capath
			|| config->default_listener.certfile
			|| config->default_listener.keyfile
			|| config->default_listener.ciphers
			|| config->default_listener.psk_hint
			|| config->default_listener.require_certificate
			|| config->default_listener.crlfile
			|| config->default_listener.use_identity_as_username
			|| config->default_listener.use_subject_as_username
#endif
			|| config->default_listener.use_username_as_clientid
			|| config->default_listener.host
			|| config->default_listener.port
			|| config->default_listener.max_connections != -1
			|| config->default_listener.mount_point
			|| config->default_listener.protocol != mp_mqtt
			|| config->default_listener.socket_domain
			|| config->default_listener.security_options.password_file
			|| config->default_listener.security_options.psk_file
			|| config->default_listener.security_options.auth_plugin_config_count
			|| config->default_listener.security_options.allow_anonymous != -1
			){

		config->listener_count++;
		config->listeners = mosquitto__realloc(config->listeners, sizeof(struct mosquitto__listener)*config->listener_count);
		if(!config->listeners){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			return MOSQ_ERR_NOMEM;
		}
		memset(&config->listeners[config->listener_count-1], 0, sizeof(struct mosquitto__listener));
		if(config->default_listener.port){
			config->listeners[config->listener_count-1].port = config->default_listener.port;
		}else{
			config->listeners[config->listener_count-1].port = 1883;
		}
		if(config->default_listener.host){
			config->listeners[config->listener_count-1].host = config->default_listener.host;
		}else{
			config->listeners[config->listener_count-1].host = NULL;
		}
		if(config->default_listener.mount_point){
			config->listeners[config->listener_count-1].mount_point = config->default_listener.mount_point;
		}else{
			config->listeners[config->listener_count-1].mount_point = NULL;
		}
		config->listeners[config->listener_count-1].max_connections = config->default_listener.max_connections;
		config->listeners[config->listener_count-1].protocol = config->default_listener.protocol;
		config->listeners[config->listener_count-1].socket_domain = config->default_listener.socket_domain;
		config->listeners[config->listener_count-1].client_count = 0;
		config->listeners[config->listener_count-1].socks = NULL;
		config->listeners[config->listener_count-1].sock_count = 0;
		config->listeners[config->listener_count-1].client_count = 0;
		config->listeners[config->listener_count-1].use_username_as_clientid = config->default_listener.use_username_as_clientid;
#ifdef WITH_TLS
		config->listeners[config->listener_count-1].tls_version = config->default_listener.tls_version;
		config->listeners[config->listener_count-1].cafile = config->default_listener.cafile;
		config->listeners[config->listener_count-1].capath = config->default_listener.capath;
		config->listeners[config->listener_count-1].certfile = config->default_listener.certfile;
		config->listeners[config->listener_count-1].keyfile = config->default_listener.keyfile;
		config->listeners[config->listener_count-1].ciphers = config->default_listener.ciphers;
		config->listeners[config->listener_count-1].psk_hint = config->default_listener.psk_hint;
		config->listeners[config->listener_count-1].require_certificate = config->default_listener.require_certificate;
		config->listeners[config->listener_count-1].ssl_ctx = NULL;
		config->listeners[config->listener_count-1].crlfile = config->default_listener.crlfile;
 		config->listeners[config->listener_count-1].use_identity_as_username = config->default_listener.use_identity_as_username;
 		config->listeners[config->listener_count-1].use_subject_as_username = config->default_listener.use_subject_as_username;
 #endif
		config->listeners[config->listener_count-1].security_options.acl_file = config->default_listener.security_options.acl_file;
 		config->listeners[config->listener_count-1].security_options.password_file = config->default_listener.security_options.password_file;
 		config->listeners[config->listener_count-1].security_options.psk_file = config->default_listener.security_options.psk_file;
 		config->listeners[config->listener_count-1].security_options.auth_plugin_configs = config->default_listener.security_options.auth_plugin_configs;
		config->listeners[config->listener_count-1].security_options.auth_plugin_config_count = config->default_listener.security_options.auth_plugin_config_count;
		config->listeners[config->listener_count-1].security_options.allow_anonymous = config->default_listener.security_options.allow_anonymous;
	}

	/* Default to drop to mosquitto user if we are privileged and no user specified. */
	if(!config->user){
		config->user = "mosquitto";
	}
	if(db->verbose){
		config->log_type = INT_MAX;
	}
	return config__check(config);
}
