PluginModule::PluginModule(const std::string& name,
                           const FilePath& path,
                           PluginDelegate::ModuleLifetime* lifetime_delegate)
    : lifetime_delegate_(lifetime_delegate),
      callback_tracker_(new ::ppapi::CallbackTracker),
      is_in_destructor_(false),
      is_crashed_(false),
      broker_(NULL),
       library_(NULL),
       name_(name),
       path_(path),
      reserve_instance_id_(NULL),
      nacl_ipc_proxy_(false) {
   if (!host_globals)
     host_globals = new HostGlobals;

  memset(&entry_points_, 0, sizeof(entry_points_));
  pp_module_ = HostGlobals::Get()->AddModule(this);
  GetMainThreadMessageLoop();  // Initialize the main thread message loop.
  GetLivePluginSet()->insert(this);
}
