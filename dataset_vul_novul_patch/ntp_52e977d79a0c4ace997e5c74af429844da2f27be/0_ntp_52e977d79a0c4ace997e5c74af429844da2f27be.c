config_monitor(
	config_tree *ptree
	)
{
	int_node *pfilegen_token;
	const char *filegen_string;
	const char *filegen_file;
	FILEGEN *filegen;
	filegen_node *my_node;
	attr_val *my_opts;
	int filegen_type;
	int filegen_flag;

	/* Set the statistics directory */
	if (ptree->stats_dir)
		stats_config(STATS_STATSDIR, ptree->stats_dir);

	/* NOTE:
	 * Calling filegen_get is brain dead. Doing a string
	 * comparison to find the relavant filegen structure is
	 * expensive.
	 *
	 * Through the parser, we already know which filegen is
	 * being specified. Hence, we should either store a
	 * pointer to the specified structure in the syntax tree
	 * or an index into a filegen array.
	 *
	 * Need to change the filegen code to reflect the above.
	 */

	/* Turn on the specified statistics */
	pfilegen_token = HEAD_PFIFO(ptree->stats_list);
 	for (; pfilegen_token != NULL; pfilegen_token = pfilegen_token->link) {
 		filegen_string = keyword(pfilegen_token->i);
 		filegen = filegen_get(filegen_string);
		if (NULL == filegen) {
			msyslog(LOG_ERR,
				"stats %s unrecognized",
				filegen_string);
			continue;
		}
 		DPRINTF(4, ("enabling filegen for %s statistics '%s%s'\n",
 			    filegen_string, filegen->prefix, 
 			    filegen->basename));
		filegen->flag |= FGEN_FLAG_ENABLED;
	}

	/* Configure the statistics with the options */
	my_node = HEAD_PFIFO(ptree->filegen_opts);
 	for (; my_node != NULL; my_node = my_node->link) {
 		filegen_file = keyword(my_node->filegen_token);
 		filegen = filegen_get(filegen_file);
		if (NULL == filegen) {
			msyslog(LOG_ERR,
				"filegen category '%s' unrecognized",
				filegen_file);
			continue;
		}
 
 		/* Initialize the filegen variables to their pre-configuration states */
 		filegen_flag = filegen->flag;
		filegen_type = filegen->type;

		/* "filegen ... enabled" is the default (when filegen is used) */
		filegen_flag |= FGEN_FLAG_ENABLED;

		my_opts = HEAD_PFIFO(my_node->options);
		for (; my_opts != NULL; my_opts = my_opts->link) {
			switch (my_opts->attr) {

			case T_File:
				filegen_file = my_opts->value.s;
				break;

			case T_Type:
				switch (my_opts->value.i) {

				default:
					NTP_INSIST(0);
					break;

				case T_None:
					filegen_type = FILEGEN_NONE;
					break;

				case T_Pid:
					filegen_type = FILEGEN_PID;
					break;

				case T_Day:
					filegen_type = FILEGEN_DAY;
					break;

				case T_Week:
					filegen_type = FILEGEN_WEEK;
					break;

				case T_Month:
					filegen_type = FILEGEN_MONTH;
					break;

				case T_Year:
					filegen_type = FILEGEN_YEAR;
					break;

				case T_Age:
					filegen_type = FILEGEN_AGE;
					break;
				}
				break;

			case T_Flag:
				switch (my_opts->value.i) {

				case T_Link:
					filegen_flag |= FGEN_FLAG_LINK;
					break;

				case T_Nolink:
					filegen_flag &= ~FGEN_FLAG_LINK;
					break;

				case T_Enable:
					filegen_flag |= FGEN_FLAG_ENABLED;
					break;

				case T_Disable:
					filegen_flag &= ~FGEN_FLAG_ENABLED;
					break;

				default:
					msyslog(LOG_ERR, 
						"Unknown filegen flag token %d",
						my_opts->value.i);
					exit(1);
				}
				break;

			default:
				msyslog(LOG_ERR,
					"Unknown filegen option token %d",
					my_opts->attr);
				exit(1);
			}
		}
		filegen_config(filegen, filegen_file, filegen_type,
			       filegen_flag);
	}
}
