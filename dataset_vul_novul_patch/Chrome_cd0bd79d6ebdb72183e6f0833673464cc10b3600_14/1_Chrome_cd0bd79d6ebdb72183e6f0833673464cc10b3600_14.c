GpuChannel::GpuChannel(GpuChannelManager* gpu_channel_manager,
                       GpuWatchdog* watchdog,
                       gfx::GLShareGroup* share_group,
                       int client_id,
                        bool software)
     : gpu_channel_manager_(gpu_channel_manager),
       client_id_(client_id),
      renderer_process_(base::kNullProcessHandle),
      renderer_pid_(base::kNullProcessId),
       share_group_(share_group ? share_group : new gfx::GLShareGroup),
       watchdog_(watchdog),
       software_(software),
      handle_messages_scheduled_(false),
      processed_get_state_fast_(false),
      num_contexts_preferring_discrete_gpu_(0),
      weak_factory_(ALLOW_THIS_IN_INITIALIZER_LIST(this)) {
  DCHECK(gpu_channel_manager);
  DCHECK(client_id);

  channel_id_ = IPC::Channel::GenerateVerifiedChannelID("gpu");
  const CommandLine* command_line = CommandLine::ForCurrentProcess();
  log_messages_ = command_line->HasSwitch(switches::kLogPluginMessages);
  disallowed_features_.multisampling =
      command_line->HasSwitch(switches::kDisableGLMultisampling);
  disallowed_features_.driver_bug_workarounds =
      command_line->HasSwitch(switches::kDisableGpuDriverBugWorkarounds);
 }
