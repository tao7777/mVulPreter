 SchedulerObject::remove(std::string key, std::string &reason, std::string &text)
 {
        PROC_ID id = getProcByString(key.c_str());
       if (id.cluster < 0 || id.proc < 0) {
                dprintf(D_FULLDEBUG, "Remove: Failed to parse id: %s\n", key.c_str());
                text = "Invalid Id";
                return false;
	}

	if (!abortJob(id.cluster,
				  id.proc,
				  reason.c_str(),
				  true // Always perform within a transaction
				  )) {
		text = "Failed to remove job";
		return false;
	}

	return true;
}
