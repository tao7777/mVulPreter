void PluginModule::InstanceDeleted(PluginInstance* instance) {
   if (out_of_process_proxy_.get())
     out_of_process_proxy_->RemoveInstance(instance->pp_instance());
   instances_.erase(instance);
  if (nacl_ipc_proxy_) {
    out_of_process_proxy_.reset();
    reserve_instance_id_ = NULL;
  }
 }
