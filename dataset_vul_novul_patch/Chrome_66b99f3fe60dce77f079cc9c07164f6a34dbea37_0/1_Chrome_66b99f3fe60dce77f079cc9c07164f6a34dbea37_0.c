 void BrowserPpapiHostImpl::DeleteInstance(PP_Instance instance) {
   auto it = instance_map_.find(instance);
  DCHECK(it != instance_map_.end());
  for (auto& observer : it->second->observer_list)
    observer.OnHostDestroyed();
 
  instance_map_.erase(it);
 }
