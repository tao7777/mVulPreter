 SchedulerObject::hold(std::string key, std::string &reason, std::string &text)
 {
        PROC_ID id = getProcByString(key.c_str());
       if (id.cluster < 0 || id.proc < 0) {
                dprintf(D_FULLDEBUG, "Hold: Failed to parse id: %s\n", key.c_str());
                text = "Invalid Id";
                return false;
	}

	if (!holdJob(id.cluster,
				 id.proc,
				 reason.c_str(),
				 true, // Always perform this action within a transaction
				 true, // Always notify the shadow of the hold
				 false, // Do not email the user about this action
				 false, // Do not email admin about this action
				 false // This is not a system related (internal) hold
				 )) {
		text = "Failed to hold job";
		return false;
	}

	return true;
}
