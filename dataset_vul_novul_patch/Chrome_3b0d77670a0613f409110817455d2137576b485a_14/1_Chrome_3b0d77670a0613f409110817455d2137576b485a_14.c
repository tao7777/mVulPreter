 PP_Bool StartPpapiProxy(PP_Instance instance) {
  if (CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNaClIPCProxy)) {
    ChannelHandleMap& map = g_channel_handle_map.Get();
    ChannelHandleMap::iterator it = map.find(instance);
    if (it == map.end())
      return PP_FALSE;
    IPC::ChannelHandle channel_handle = it->second;
    map.erase(it);
    webkit::ppapi::PluginInstance* plugin_instance =
        content::GetHostGlobals()->GetInstance(instance);
    if (!plugin_instance)
      return PP_FALSE;
    WebView* web_view =
        plugin_instance->container()->element().document().frame()->view();
    RenderView* render_view = content::RenderView::FromWebView(web_view);
    webkit::ppapi::PluginModule* plugin_module = plugin_instance->module();
    scoped_refptr<SyncMessageStatusReceiver>
        status_receiver(new SyncMessageStatusReceiver());
    scoped_ptr<OutOfProcessProxy> out_of_process_proxy(new OutOfProcessProxy);
    if (out_of_process_proxy->Init(
            channel_handle,
            plugin_module->pp_module(),
            webkit::ppapi::PluginModule::GetLocalGetInterfaceFunc(),
            ppapi::Preferences(render_view->GetWebkitPreferences()),
            status_receiver.get())) {
      plugin_module->InitAsProxiedNaCl(
          out_of_process_proxy.PassAs<PluginDelegate::OutOfProcessProxy>(),
          instance);
      return PP_TRUE;
    }
  }
   return PP_FALSE;
 }
