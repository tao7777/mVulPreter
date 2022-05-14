void DaemonProcess::OnChannelConnected() {
   DCHECK(caller_task_runner()->BelongsToCurrentThread());
 
   DeleteAllDesktopSessions();

  next_terminal_id_ = 0;

  SendToNetwork(
      new ChromotingDaemonNetworkMsg_Configuration(serialized_config_));
}
