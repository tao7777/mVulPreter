void PluginInstance::OnTimerCall(void (*func)(NPP id, uint32 timer_id),
                                 NPP id,
                                 uint32 timer_id) {
  TimerMap::iterator it = timers_.find(timer_id);
  if (it == timers_.end())
    return;

  TimerInfo info = it->second;

  func(id, timer_id);

  if (timers_.find(timer_id) == timers_.end())
    return;

   if (info.repeat) {
    MessageLoop::current()->PostDelayedTask(
        FROM_HERE,
        NewRunnableMethod(
            this, &PluginInstance::OnTimerCall, func, npp_, timer_id),
        info.interval);
   } else {
     timers_.erase(it);
   }
}
