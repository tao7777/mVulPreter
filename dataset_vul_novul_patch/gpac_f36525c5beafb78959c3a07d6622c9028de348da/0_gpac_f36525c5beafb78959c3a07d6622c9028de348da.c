GF_Err gf_bin128_parse(const char *string, bin128 value)
{
	u32 len;
	u32	i=0;
	if (!strnicmp(string, "0x", 2)) string += 2;
	len = (u32) strlen(string);
	if (len >= 32) {
		u32 j;
		for (j=0; j<len; j+=2) {
			u32 v;
			char szV[5];

			while (string[j] && !isalnum(string[j]))
				j++;
			if (!string[j])
				break;
			sprintf(szV, "%c%c", string[j], string[j+1]);
 			sscanf(szV, "%x", &v);
 			value[i] = v;
 			i++;
			if (i > 15) {
				// force error check below
				i++;
				break;
			}
 		}
 	}
 	if (i != 16) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[CORE] 128bit blob is not 16-bytes long: %s\n", string));
		return GF_BAD_PARAM;
	}
	return GF_OK;
}
