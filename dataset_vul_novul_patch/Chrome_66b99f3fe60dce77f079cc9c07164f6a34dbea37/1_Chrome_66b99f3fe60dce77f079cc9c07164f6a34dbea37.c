 void BrowserPpapiHostImpl::AddInstance(
     PP_Instance instance,
     const PepperRendererInstanceData& renderer_instance_data) {
  DCHECK(instance_map_.find(instance) == instance_map_.end());
  instance_map_[instance] =
      base::MakeUnique<InstanceData>(renderer_instance_data);
 }
