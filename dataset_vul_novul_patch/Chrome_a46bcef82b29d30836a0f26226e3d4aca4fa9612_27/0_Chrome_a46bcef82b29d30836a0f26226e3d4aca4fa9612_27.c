 void ScreenRecorder::AddConnection(
     scoped_refptr<ConnectionToClient> connection) {
  DCHECK(network_loop_->BelongsToCurrentThread());
  connections_.push_back(connection);

   capture_loop_->PostTask(
       FROM_HERE, base::Bind(&ScreenRecorder::DoInvalidateFullScreen, this));
 }
