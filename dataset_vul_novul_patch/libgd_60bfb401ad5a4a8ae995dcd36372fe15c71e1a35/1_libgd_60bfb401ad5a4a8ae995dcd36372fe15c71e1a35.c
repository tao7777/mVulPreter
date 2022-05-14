static inline LineContribType * _gdContributionsAlloc(unsigned int line_length, unsigned int windows_size)
 {
 	unsigned int u = 0;
 	LineContribType *res;
	int overflow_error = 0;
 
 	res = (LineContribType *) gdMalloc(sizeof(LineContribType));
 	if (!res) {
 		return NULL;
	}
	res->WindowSize = windows_size;
	res->LineLength = line_length;
	if (overflow2(line_length, sizeof(ContributionType))) {
		gdFree(res);
		return NULL;
	}
	res->ContribRow = (ContributionType *) gdMalloc(line_length * sizeof(ContributionType));
	if (res->ContribRow == NULL) {
		gdFree(res);
 		return NULL;
 	}
 	for (u = 0 ; u < line_length ; u++) {
		if (overflow2(windows_size, sizeof(double))) {
			overflow_error = 1;
		} else {
			res->ContribRow[u].Weights = (double *) gdMalloc(windows_size * sizeof(double));
		}
		if (overflow_error == 1 || res->ContribRow[u].Weights == NULL) {
 			unsigned int i;
			u--;
			for (i=0;i<=u;i++) {
 				gdFree(res->ContribRow[i].Weights);
 			}
 			gdFree(res->ContribRow);
			gdFree(res);
			return NULL;
		}
	}
	return res;
}
