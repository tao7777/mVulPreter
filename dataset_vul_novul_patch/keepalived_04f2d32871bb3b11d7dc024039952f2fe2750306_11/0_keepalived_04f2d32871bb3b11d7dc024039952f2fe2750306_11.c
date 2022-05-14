dump_keywords(vector_t *keydump, int level, FILE *fp)
 {
 	unsigned int i;
 	keyword_t *keyword_vec;
	char file_name[22];
 
 	if (!level) {
 		snprintf(file_name, sizeof(file_name), "/tmp/keywords.%d", getpid());
		fp = fopen_safe(file_name, "w");
 		if (!fp)
 			return;
 	}

	for (i = 0; i < vector_size(keydump); i++) {
		keyword_vec = vector_slot(keydump, i);
		fprintf(fp, "%*sKeyword : %s (%s)\n", level * 2, "", keyword_vec->string, keyword_vec->active ? "active": "disabled");
		if (keyword_vec->sub)
			dump_keywords(keyword_vec->sub, level + 1, fp);
	}

	if (!level)
		fclose(fp);
}
