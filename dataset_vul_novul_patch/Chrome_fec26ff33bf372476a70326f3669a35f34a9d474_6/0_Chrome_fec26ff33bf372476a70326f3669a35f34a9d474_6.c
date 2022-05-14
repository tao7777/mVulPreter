void PreconnectManager::FinishPreresolveJob(PreresolveJobId job_id,
                                            bool success) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  PreresolveJob* job = preresolve_jobs_.Lookup(job_id);
  DCHECK(job);

  bool need_preconnect = success && job->need_preconnect();
  if (need_preconnect) {
    PreconnectUrl(job->url, job->num_sockets, job->allow_credentials,
                  job->network_isolation_key);
   }
 
   PreresolveInfo* info = job->info;
  if (info) {
    info->stats->requests_stats.emplace_back(url::Origin::Create(job->url),
                                             need_preconnect);
  }
   preresolve_jobs_.Remove(job_id);
   --inflight_preresolves_count_;
   if (info) {
    DCHECK_LE(1u, info->inflight_count);
    --info->inflight_count;
  }
  if (info && info->is_done())
    AllPreresolvesForUrlFinished(info);
  TryToLaunchPreresolveJobs();
}
