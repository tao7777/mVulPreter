 void add_param_to_argv(char *parsestart, int line)
 {
	int quote_open = 0, escaped = 0;
	struct xt_param_buf param = {};
	char *curchar;
 
 	/* After fighting with strtok enough, here's now
 	 * a 'real' parser. According to Rusty I'm now no
			} else {
				param_buffer[param_len++] = *curchar;
 	for (curchar = parsestart; *curchar; curchar++) {
 		if (quote_open) {
 			if (escaped) {
				add_param(&param, curchar);
 				escaped = 0;
 				continue;
 			} else if (*curchar == '\\') {
		}

		switch (*curchar) {
 				quote_open = 0;
 				*curchar = '"';
 			} else {
				add_param(&param, curchar);
 				continue;
 			}
 		} else {
				continue;
			}
			break;
		default:
			/* regular character, copy to buffer */
			param_buffer[param_len++] = *curchar;

			if (param_len >= sizeof(param_buffer))
				xtables_error(PARAMETER_PROBLEM,
 		case ' ':
 		case '\t':
 		case '\n':
			if (!param.len) {
 				/* two spaces? */
 				continue;
 			}
 			break;
 		default:
 			/* regular character, copy to buffer */
			add_param(&param, curchar);
 			continue;
 		}
 
		param.buffer[param.len] = '\0';
 
 		/* check if table name specified */
		if ((param.buffer[0] == '-' &&
		     param.buffer[1] != '-' &&
		     strchr(param.buffer, 't')) ||
		    (!strncmp(param.buffer, "--t", 3) &&
		     !strncmp(param.buffer, "--table", strlen(param.buffer)))) {
 			xtables_error(PARAMETER_PROBLEM,
 				      "The -t option (seen in line %u) cannot be used in %s.\n",
 				      line, xt_params->program_name);
 		}
 
		add_argv(param.buffer, 0);
		param.len = 0;
 	}
