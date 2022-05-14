static inline LineContribType * _gdContributionsAlloc(unsigned int line_length, unsigned int windows_size)
 {
 	unsigned int u = 0;
 	LineContribType *res;
	size_t weights_size;
 
	if (overflow2(windows_size, sizeof(double))) {
		return NULL;
	} else {
		weights_size = windows_size * sizeof(double);
	}
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
		res->ContribRow[u].Weights = (double *) gdMalloc(weights_size);
		if (res->ContribRow[u].Weights == NULL) {
 			unsigned int i;

			for (i=0;i<u;i++) {
 				gdFree(res->ContribRow[i].Weights);
 			}
 			gdFree(res->ContribRow);
			gdFree(res);
			return NULL;
		}
	}
	return res;
}
