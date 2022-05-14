void set_cfg_option(char *opt_string)
{
	char *sep, *sep2, szSec[1024], szKey[1024], szVal[1024];
	sep = strchr(opt_string, ':');
	if (!sep) {
		fprintf(stderr, "Badly formatted option %s - expected Section:Name=Value\n", opt_string);
		return;
 	}
 	{
 		const size_t sepIdx = sep - opt_string;
 		strncpy(szSec, opt_string, sepIdx);
 		szSec[sepIdx] = 0;
 	}
	sep ++;
	sep2 = strchr(sep, '=');
	if (!sep2) {
		fprintf(stderr, "Badly formatted option %s - expected Section:Name=Value\n", opt_string);
		return;
 	}
 	{
 		const size_t sepIdx = sep2 - sep;
 		strncpy(szKey, sep, sepIdx);
 		szKey[sepIdx] = 0;
 		strcpy(szVal, sep2+1);
 	}
 
	if (!stricmp(szKey, "*")) {
		if (stricmp(szVal, "null")) {
			fprintf(stderr, "Badly formatted option %s - expected Section:*=null\n", opt_string);
			return;
		}
		gf_cfg_del_section(cfg_file, szSec);
		return;
	}

	if (!stricmp(szVal, "null")) {
		szVal[0]=0;
	}
	gf_cfg_set_key(cfg_file, szSec, szKey, szVal[0] ? szVal : NULL);
}
