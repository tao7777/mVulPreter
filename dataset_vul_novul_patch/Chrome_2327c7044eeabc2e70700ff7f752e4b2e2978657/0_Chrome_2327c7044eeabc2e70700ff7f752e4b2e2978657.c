  void InitOnIOThread(const std::string& mime_type) {
    PluginServiceImpl* plugin_service = PluginServiceImpl::GetInstance();

     std::vector<WebPluginInfo> plugins;
     plugin_service->GetPluginInfoArray(
         GURL(), mime_type, false, &plugins, NULL);

    if (plugins.empty()) {
      // May be empty for some tests and on the CrOS login OOBE screen.
      event_->Signal();
      return;
    }

    base::FilePath plugin_path = plugins[0].path;
 
     DCHECK_CURRENTLY_ON(BrowserThread::IO);
     remove_start_time_ = base::Time::Now();
    is_removing_ = true;
    AddRef();

    PepperPluginInfo* pepper_info =
        plugin_service->GetRegisteredPpapiPluginInfo(plugin_path);
    if (pepper_info) {
      plugin_name_ = pepper_info->name;
      plugin_service->OpenChannelToPpapiBroker(0, plugin_path, this);
    } else {
      plugin_service->OpenChannelToNpapiPlugin(
          0, 0, GURL(), GURL(), mime_type, this);
    }
  }
