 SchedulerObject::release(std::string key, std::string &reason, std::string &text)
 {
        PROC_ID id = getProcByString(key.c_str());
       if (id.cluster <= 0 || id.proc < 0) {
                dprintf(D_FULLDEBUG, "Release: Failed to parse id: %s\n", key.c_str());
                text = "Invalid Id";
                return false;
	}

	if (!releaseJob(id.cluster,
					id.proc,
					reason.c_str(),
					true, // Always perform this action within a transaction
					false, // Do not email the user about this action
					false // Do not email admin about this action
					)) {
		text = "Failed to release job";
		return false;
	}

	return true;
}
