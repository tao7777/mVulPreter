PluginInstance::PluginInstance(PluginLib *plugin, const std::string &mime_type)
    : plugin_(plugin),
      npp_(0),
      host_(PluginHost::Singleton()),
      npp_functions_(plugin->functions()),
      window_handle_(0),
      windowless_(false),
      transparent_(true),
      webplugin_(0),
      mime_type_(mime_type),
      use_mozilla_user_agent_(false),
#if defined (OS_MACOSX)
      drawing_model_(0),
       event_model_(0),
       currently_handled_event_(NULL),
 #endif
      message_loop_(MessageLoop::current()),
       load_manually_(false),
       in_close_streams_(false),
      next_timer_id_(1) {
   npp_ = new NPP_t();
   npp_->ndata = 0;
   npp_->pdata = 0;
 
   memset(&zero_padding_, 0, sizeof(zero_padding_));
  DCHECK(message_loop_);
 }
