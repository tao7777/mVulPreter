void ScreenRecorder::DoRemoveClient(
    scoped_refptr<ConnectionToClient> connection) {
  DCHECK(network_loop_->BelongsToCurrentThread());
  ConnectionToClientList::iterator it =
      std::find(connections_.begin(), connections_.end(), connection);
  if (it != connections_.end()) {
    connections_.erase(it);
  }
}
