void AppCacheUpdateJob::StartUpdate(AppCacheHost* host,
                                    const GURL& new_master_resource) {
  DCHECK(group_->update_job() == this);
  DCHECK(!group_->is_obsolete());

  bool is_new_pending_master_entry = false;
  if (!new_master_resource.is_empty()) {
    DCHECK(new_master_resource == host->pending_master_entry_url());
     DCHECK(!new_master_resource.has_ref());
     DCHECK(new_master_resource.GetOrigin() == manifest_url_.GetOrigin());
 
     if (IsTerminating()) {
       group_->QueueUpdate(host, new_master_resource);
      return;
    }

    std::pair<PendingMasters::iterator, bool> ret =
        pending_master_entries_.insert(
            PendingMasters::value_type(new_master_resource, PendingHosts()));
    is_new_pending_master_entry = ret.second;
    ret.first->second.push_back(host);
    host->AddObserver(this);
  }

  AppCacheGroup::UpdateAppCacheStatus update_status = group_->update_status();
  if (update_status == AppCacheGroup::CHECKING ||
      update_status == AppCacheGroup::DOWNLOADING) {
    if (host) {
      NotifySingleHost(host, APPCACHE_CHECKING_EVENT);
      if (update_status == AppCacheGroup::DOWNLOADING)
        NotifySingleHost(host, APPCACHE_DOWNLOADING_EVENT);

      if (!new_master_resource.is_empty()) {
        AddMasterEntryToFetchList(host, new_master_resource,
                                  is_new_pending_master_entry);
      }
    }
    return;
  }

  MadeProgress();
  group_->SetUpdateAppCacheStatus(AppCacheGroup::CHECKING);
  if (group_->HasCache()) {
    base::TimeDelta kFullUpdateInterval = base::TimeDelta::FromHours(24);
    update_type_ = UPGRADE_ATTEMPT;
    base::TimeDelta time_since_last_check =
        base::Time::Now() - group_->last_full_update_check_time();
    doing_full_update_check_ = time_since_last_check > kFullUpdateInterval;
    NotifyAllAssociatedHosts(APPCACHE_CHECKING_EVENT);
  } else {
    update_type_ = CACHE_ATTEMPT;
    doing_full_update_check_ = true;
    DCHECK(host);
    NotifySingleHost(host, APPCACHE_CHECKING_EVENT);
  }

  if (!new_master_resource.is_empty()) {
    AddMasterEntryToFetchList(host, new_master_resource,
                              is_new_pending_master_entry);
  }

  BrowserThread::PostAfterStartupTask(
      FROM_HERE, base::ThreadTaskRunnerHandle::Get(),
      base::Bind(&AppCacheUpdateJob::FetchManifest, weak_factory_.GetWeakPtr(),
                 true));
}
