 static int asymmetric_key_match_preparse(struct key_match_data *match_data)
 {
 	match_data->lookup_type = KEYRING_SEARCH_LOOKUP_ITERATE;
	match_data->cmp = asymmetric_key_cmp;
 	return 0;
 }
