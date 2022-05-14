void DaemonProcess::OnChannelConnected() {
void DaemonProcess::OnChannelConnected(int32 peer_pid) {
   DCHECK(caller_task_runner()->BelongsToCurrentThread());
 
   DeleteAllDesktopSessions();

  next_terminal_id_ = 0;

  SendToNetwork(
      new ChromotingDaemonNetworkMsg_Configuration(serialized_config_));
}
