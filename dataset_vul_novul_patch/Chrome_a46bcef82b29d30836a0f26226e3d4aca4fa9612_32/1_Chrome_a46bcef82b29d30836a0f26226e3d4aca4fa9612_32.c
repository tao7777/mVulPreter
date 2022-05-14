 void ScreenRecorder::RemoveConnection(
     scoped_refptr<ConnectionToClient> connection) {
  network_loop_->PostTask(
      FROM_HERE, base::Bind(&ScreenRecorder::DoRemoveClient, this, connection));
 }
