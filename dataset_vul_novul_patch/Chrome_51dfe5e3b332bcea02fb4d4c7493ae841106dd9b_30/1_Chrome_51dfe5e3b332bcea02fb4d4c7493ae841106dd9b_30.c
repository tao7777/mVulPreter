void PulseAudioMixer::PulseAudioFree() {
   {
     AutoLock lock(mixer_state_lock_);
     if (!pa_mainloop_)
       mixer_state_ = UNINITIALIZED;
     if ((mixer_state_ == UNINITIALIZED) || (mixer_state_ == SHUTTING_DOWN))
       return;
     mixer_state_ = SHUTTING_DOWN;
   }

  DCHECK(pa_mainloop_);

  MainloopLock();
  if (pa_context_) {
    pa_context_disconnect(pa_context_);
    pa_context_unref(pa_context_);
    pa_context_ = NULL;
  }
  MainloopUnlock();

  pa_threaded_mainloop_stop(pa_mainloop_);
  pa_threaded_mainloop_free(pa_mainloop_);
  pa_mainloop_ = NULL;

  {
    AutoLock lock(mixer_state_lock_);
    mixer_state_ = UNINITIALIZED;
   }
 }
