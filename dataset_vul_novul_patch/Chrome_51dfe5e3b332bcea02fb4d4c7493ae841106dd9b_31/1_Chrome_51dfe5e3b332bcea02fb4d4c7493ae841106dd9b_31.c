bool PulseAudioMixer::PulseAudioInit() {
   pa_context_state_t state = PA_CONTEXT_FAILED;
 
   {
    AutoLock lock(mixer_state_lock_);
    if (mixer_state_ != INITIALIZING)
      return false;

    pa_mainloop_ = pa_threaded_mainloop_new();
    if (!pa_mainloop_) {
      LOG(ERROR) << "Can't create PulseAudio mainloop";
      mixer_state_ = UNINITIALIZED;
      return false;
    }

    if (pa_threaded_mainloop_start(pa_mainloop_) != 0) {
      LOG(ERROR) << "Can't start PulseAudio mainloop";
      pa_threaded_mainloop_free(pa_mainloop_);
      mixer_state_ = UNINITIALIZED;
      return false;
    }
  }

  while (true) {
    if (!MainloopSafeLock())
      return false;

    while (true) {
      pa_mainloop_api* pa_mlapi = pa_threaded_mainloop_get_api(pa_mainloop_);
      if (!pa_mlapi) {
        LOG(ERROR) << "Can't get PulseAudio mainloop api";
        break;
      }
      pa_context_ = pa_context_new(pa_mlapi, "ChromeAudio");
      if (!pa_context_) {
        LOG(ERROR) << "Can't create new PulseAudio context";
        break;
      }

      MainloopUnlock();
      if (!MainloopSafeLock())
        return false;

      CallbackWrapper data = {this, false, NULL};
      pa_context_set_state_callback(pa_context_,
                                    &ConnectToPulseCallbackThunk,
                                    &data);

      if (pa_context_connect(pa_context_, NULL,
                             PA_CONTEXT_NOAUTOSPAWN, NULL) != 0) {
        LOG(ERROR) << "Can't start connection to PulseAudio sound server";
      } else {
        do {
          MainloopWait();
        } while (!data.done);

        state = pa_context_get_state(pa_context_);

        if (state == PA_CONTEXT_FAILED) {
          LOG(ERROR) << "PulseAudio connection failed (daemon not running?)";
        } else if (state == PA_CONTEXT_TERMINATED) {
          LOG(ERROR) << "PulseAudio connection terminated early";
        } else if (state != PA_CONTEXT_READY) {
          LOG(ERROR) << "Unknown problem connecting to PulseAudio";
        }
      }

      pa_context_set_state_callback(pa_context_, NULL, NULL);
      break;
    }

    MainloopUnlock();

    if (state != PA_CONTEXT_READY)
      break;

    if (!MainloopSafeLock())
      return false;
    GetDefaultPlaybackDevice();
    MainloopUnlock();

    if (device_id_ == kInvalidDeviceId)
      break;

    {
      AutoLock lock(mixer_state_lock_);
      if (mixer_state_ == SHUTTING_DOWN)
        return false;
      mixer_state_ = READY;
    }

    return true;
  }

  PulseAudioFree();
   return false;
 }
