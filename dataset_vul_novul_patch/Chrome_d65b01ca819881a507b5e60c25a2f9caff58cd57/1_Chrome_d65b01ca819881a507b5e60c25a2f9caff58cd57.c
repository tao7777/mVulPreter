  AvailableSpaceQueryTask(
      QuotaManager* manager,
      const AvailableSpaceCallback& callback)
      : QuotaThreadTask(manager, manager->db_thread_),
        profile_path_(manager->profile_path_),
        space_(-1),
        get_disk_space_fn_(manager->get_disk_space_fn_),
        callback_(callback) {
    DCHECK(get_disk_space_fn_);
  }
