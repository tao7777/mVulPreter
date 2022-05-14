 PluginChannel::~PluginChannel() {
  if (renderer_handle_)
    base::CloseProcessHandle(renderer_handle_);
   MessageLoop::current()->PostDelayedTask(
       FROM_HERE,
       base::Bind(&PluginReleaseCallback),
      base::TimeDelta::FromMinutes(kPluginReleaseTimeMinutes));
}
