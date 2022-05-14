 PluginChannel::~PluginChannel() {
   MessageLoop::current()->PostDelayedTask(
       FROM_HERE,
       base::Bind(&PluginReleaseCallback),
      base::TimeDelta::FromMinutes(kPluginReleaseTimeMinutes));
}
