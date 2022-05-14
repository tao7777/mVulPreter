void ScreenRecorder::DoRemoveAllClients() {
  DCHECK(network_loop_->BelongsToCurrentThread());
  connections_.clear();
}
