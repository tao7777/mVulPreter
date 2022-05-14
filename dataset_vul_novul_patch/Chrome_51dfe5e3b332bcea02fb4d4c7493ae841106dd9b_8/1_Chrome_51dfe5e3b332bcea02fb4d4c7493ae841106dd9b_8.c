void PulseAudioMixer::CompleteOperation(pa_operation* pa_op,
                                         bool* done) const {
  CHECK(pa_op);

  while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING) {
    if (*done) {
      pa_operation_cancel(pa_op);
      break;
    }
    MainloopWait();
  }
  pa_operation_unref(pa_op);
 }
