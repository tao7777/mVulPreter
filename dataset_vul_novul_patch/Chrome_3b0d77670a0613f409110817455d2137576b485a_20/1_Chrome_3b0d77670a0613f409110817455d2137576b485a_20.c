 void PluginModule::InitAsProxiedNaCl(
    scoped_ptr<PluginDelegate::OutOfProcessProxy> out_of_process_proxy,
     PP_Instance instance) {
  nacl_ipc_proxy_ = true;
  InitAsProxied(out_of_process_proxy.release());
  out_of_process_proxy_->AddInstance(instance);

  PluginInstance* plugin_instance = host_globals->GetInstance(instance);
  if (!plugin_instance)
    return;
  plugin_instance->ResetAsProxied();
}
