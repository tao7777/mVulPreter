int jas_memdump(FILE *out, void *data, size_t len)
 {
 	size_t i;
 	size_t j;
	jas_uchar *dp;
 	dp = data;
 	for (i = 0; i < len; i += 16) {
 		fprintf(out, "%04zx:", i);
		for (j = 0; j < 16; ++j) {
			if (i + j < len) {
				fprintf(out, " %02x", dp[i + j]);
			}
		}
		fprintf(out, "\n");
	}
	return 0;
}
