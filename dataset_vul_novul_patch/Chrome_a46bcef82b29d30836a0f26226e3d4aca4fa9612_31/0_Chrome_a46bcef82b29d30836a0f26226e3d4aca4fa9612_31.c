 void ScreenRecorder::RemoveAllConnections() {
  DCHECK(network_loop_->BelongsToCurrentThread());
  connections_.clear();
 }
