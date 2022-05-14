 void ScreenRecorder::RemoveAllConnections() {
  network_loop_->PostTask(
      FROM_HERE, base::Bind(&ScreenRecorder::DoRemoveAllClients, this));
 }
