 void BrowserPpapiHostImpl::AddInstance(
     PP_Instance instance,
     const PepperRendererInstanceData& renderer_instance_data) {
  // NOTE: 'instance' may be coming from a compromised renderer process. We
  // take care here to make sure an attacker can't overwrite data for an
  // existing plugin instance.
  // See http://crbug.com/733548.
  if (instance_map_.find(instance) == instance_map_.end()) {
    instance_map_[instance] =
        base::MakeUnique<InstanceData>(renderer_instance_data);
  } else {
    NOTREACHED();
  }
 }
