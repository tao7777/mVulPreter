 void PluginModule::InitAsProxiedNaCl(
    PluginDelegate::OutOfProcessProxy* out_of_process_proxy,
     PP_Instance instance) {
  InitAsProxied(out_of_process_proxy);
  out_of_process_proxy_->AddInstance(instance);

  PluginInstance* plugin_instance = host_globals->GetInstance(instance);
  if (!plugin_instance)
    return;
  plugin_instance->ResetAsProxied();
}
