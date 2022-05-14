 void BrowserPpapiHostImpl::DeleteInstance(PP_Instance instance) {
  // NOTE: 'instance' may be coming from a compromised renderer process. We
  // take care here to make sure an attacker can't cause a UAF by deleting a
  // non-existent plugin instance.
  // See http://crbug.com/733548.
   auto it = instance_map_.find(instance);
  if (it != instance_map_.end()) {
    // We need to tell the observers for that instance that we are destroyed
    // because we won't have the opportunity to once we remove them from the
    // |instance_map_|. If the instance was deleted, observers for those
    // instances should never call back into the host anyway, so it is safe to
    // tell them that the host is destroyed.
    for (auto& observer : it->second->observer_list)
      observer.OnHostDestroyed();
 
    instance_map_.erase(it);
  } else {
    NOTREACHED();
  }
 }
