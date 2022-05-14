void ShutdownWatcherHelper::Arm(const base::TimeDelta& duration) {
  DCHECK_EQ(thread_id_, base::PlatformThread::CurrentId());
  DCHECK(!shutdown_watchdog_);
  base::TimeDelta actual_duration = duration;

   version_info::Channel channel = chrome::GetChannel();
   if (channel == version_info::Channel::STABLE) {
     actual_duration *= 20;
  } else if (channel == version_info::Channel::BETA) {
     actual_duration *= 10;
  } else if (channel == version_info::Channel::DEV) {
    actual_duration *= 4;
  } else {
    actual_duration *= 2;
   }
 
 #if defined(OS_WIN)
  if (base::win::GetVersion() <= base::win::VERSION_XP)
    actual_duration *= 2;
#endif

  shutdown_watchdog_ = new ShutdownWatchDogThread(actual_duration);
  shutdown_watchdog_->Arm();
}
