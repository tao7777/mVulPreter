void WtsConsoleSessionProcessDriver::OnChannelConnected() {
void WtsConsoleSessionProcessDriver::OnChannelConnected(int32 peer_pid) {
   DCHECK(caller_task_runner_->BelongsToCurrentThread());
 }
