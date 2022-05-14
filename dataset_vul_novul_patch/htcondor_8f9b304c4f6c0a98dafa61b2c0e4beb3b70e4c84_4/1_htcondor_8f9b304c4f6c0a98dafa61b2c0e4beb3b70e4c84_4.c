SchedulerObject::setAttribute(std::string key,
							  std::string name,
							  std::string value,
                                                          std::string &text)
 {
        PROC_ID id = getProcByString(key.c_str());
       if (id.cluster < 0 || id.proc < 0) {
                dprintf(D_FULLDEBUG, "SetAttribute: Failed to parse id: %s\n", key.c_str());
                text = "Invalid Id";
                return false;
	}

	if (isSubmissionChange(name.c_str())) {
		text = "Changes to submission name not allowed";
		return false;
	}

	if (isKeyword(name.c_str())) {
		text = "Attribute name is reserved: " + name;
		return false;
	}

	if (!isValidAttributeName(name,text)) {
		return false;
	}

	if (::SetAttribute(id.cluster,
					   id.proc,
					   name.c_str(),
					   value.c_str())) {
		text = "Failed to set attribute " + name + " to " + value;
		return false;
	}

	return true;
}
