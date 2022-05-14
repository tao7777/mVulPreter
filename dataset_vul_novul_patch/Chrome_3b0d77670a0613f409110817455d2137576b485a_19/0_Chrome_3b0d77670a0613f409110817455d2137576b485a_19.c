Plugin::Plugin(PP_Instance pp_instance)
    : pp::InstancePrivate(pp_instance),
      scriptable_plugin_(NULL),
      argc_(-1),
      argn_(NULL),
      argv_(NULL),
      main_subprocess_("main subprocess", NULL, NULL),
      nacl_ready_state_(UNSENT),
      nexe_error_reported_(false),
      wrapper_factory_(NULL),
      last_error_string_(""),
      ppapi_proxy_(NULL),
      enable_dev_interfaces_(false),
       init_time_(0),
       ready_time_(0),
       nexe_size_(0),
      time_of_last_progress_event_(0) {
   PLUGIN_PRINTF(("Plugin::Plugin (this=%p, pp_instance=%"
                  NACL_PRId32")\n", static_cast<void*>(this), pp_instance));
   callback_factory_.Initialize(this);
  nexe_downloader_.Initialize(this);
}
