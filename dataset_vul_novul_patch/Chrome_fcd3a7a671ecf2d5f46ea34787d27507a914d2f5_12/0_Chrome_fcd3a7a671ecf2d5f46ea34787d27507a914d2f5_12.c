void SyncManager::SyncInternal::UpdateEnabledTypes() {
  DCHECK(thread_checker_.CalledOnValidThread());
  ModelSafeRoutingInfo routes;
   registrar_->GetModelSafeRoutingInfo(&routes);
   const ModelTypeSet enabled_types = GetRoutingInfoTypes(routes);
   sync_notifier_->UpdateEnabledTypes(enabled_types);
