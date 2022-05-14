AviaryScheddPlugin::processJob(const char *key,
							 const char *,
							 int )
{
	PROC_ID id;
	ClassAd *jobAd;

	if (!IS_JOB(key)) return false;

 
        id = getProcByString(key);
       if (id.cluster < 0 || id.proc < 0) {
                dprintf(D_FULLDEBUG, "Failed to parse key: %s - skipping\n", key);
                return false;
        }

	if (NULL == (jobAd = ::GetJobAd(id.cluster, id.proc, false))) {
		dprintf(D_ALWAYS,
				"NOTICE: Failed to lookup ad for %s - maybe deleted\n",
				key);
		return false;
	}



	MyString submissionName;
	if (GetAttributeString(id.cluster, id.proc,
						   ATTR_JOB_SUBMISSION,
						   submissionName) < 0) {

		PROC_ID dagman;
		if (GetAttributeInt(id.cluster, id.proc,
							ATTR_DAGMAN_JOB_ID,
							&dagman.cluster) >= 0) {
			dagman.proc = 0;

			if (GetAttributeString(dagman.cluster, dagman.proc,
								   ATTR_JOB_SUBMISSION,
								   submissionName) < 0) {
				submissionName.sprintf("%s#%d", Name, dagman.cluster);
			}
		} else {
			submissionName.sprintf("%s#%d", Name, id.cluster);
		}

		MyString tmp;
		tmp += "\"";
		tmp += submissionName;
		tmp += "\"";
		SetAttribute(id.cluster, id.proc,
					 ATTR_JOB_SUBMISSION,
					 tmp.Value());
	}

	return true;
}
