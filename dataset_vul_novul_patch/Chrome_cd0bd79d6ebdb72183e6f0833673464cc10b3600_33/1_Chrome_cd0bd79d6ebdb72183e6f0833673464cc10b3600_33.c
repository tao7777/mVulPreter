 PluginChannel::PluginChannel()
    : renderer_handle_(0),
      renderer_id_(-1),
       in_send_(0),
       incognito_(false),
       filter_(new MessageFilter()) {
  set_send_unblocking_only_during_unblock_dispatch();
  ChildProcess::current()->AddRefProcess();
  const CommandLine* command_line = CommandLine::ForCurrentProcess();
  log_messages_ = command_line->HasSwitch(switches::kLogPluginMessages);
 }
