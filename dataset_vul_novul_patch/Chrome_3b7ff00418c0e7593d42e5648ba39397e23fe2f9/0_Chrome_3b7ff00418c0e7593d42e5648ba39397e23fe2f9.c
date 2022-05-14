void SyncBackendHost::Shutdown(bool sync_disabled) {
   if (sync_thread_.IsRunning()) {  // Not running in tests.
    if (core_->sync_manager()) {
       core_->sync_manager()->RequestEarlyExit();
     }
     sync_thread_.message_loop()->PostTask(FROM_HERE,
        NewRunnableMethod(core_.get(),
                          &SyncBackendHost::Core::DoShutdown,
                          sync_disabled));
  }

  if (registrar_.get()) {
    registrar_->StopOnUIThread();
  }

  {
    base::ThreadRestrictions::ScopedAllowIO allow_io;
    sync_thread_.Stop();
  }

  registrar_.reset();
  frontend_ = NULL;
  core_ = NULL;  // Releases reference to core_.
}
