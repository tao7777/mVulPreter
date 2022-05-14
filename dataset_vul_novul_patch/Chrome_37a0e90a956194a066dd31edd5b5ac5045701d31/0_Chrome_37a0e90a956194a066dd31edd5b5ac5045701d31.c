ChildProcessTerminationInfo ChildProcessLauncherHelper::GetTerminationInfo(
    const ChildProcessLauncherHelper::Process& process,
    bool known_dead) {
  ChildProcessTerminationInfo info;
   if (!java_peer_avaiable_on_client_thread_)
     return info;
 
  Java_ChildProcessLauncherHelperImpl_getTerminationInfoAndStop(
       AttachCurrentThread(), java_peer_, reinterpret_cast<intptr_t>(&info));
 
   base::android::ApplicationState app_state =
      base::android::ApplicationStatusListener::GetState();
  bool app_foreground =
      app_state == base::android::APPLICATION_STATE_HAS_RUNNING_ACTIVITIES ||
      app_state == base::android::APPLICATION_STATE_HAS_PAUSED_ACTIVITIES;

  if (app_foreground &&
      (info.binding_state == base::android::ChildBindingState::MODERATE ||
       info.binding_state == base::android::ChildBindingState::STRONG)) {
    info.status = base::TERMINATION_STATUS_OOM_PROTECTED;
  } else {
    info.status = base::TERMINATION_STATUS_NORMAL_TERMINATION;
  }
  return info;
}
