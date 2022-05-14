void AppCacheUpdateJob::OnDestructionImminent(AppCacheHost* host) {
   PendingMasters::iterator found =
       pending_master_entries_.find(host->pending_master_entry_url());
  DCHECK(found != pending_master_entries_.end());
   PendingHosts& hosts = found->second;
   PendingHosts::iterator it = std::find(hosts.begin(), hosts.end(), host);
  DCHECK(it != hosts.end());
   hosts.erase(it);
 }
