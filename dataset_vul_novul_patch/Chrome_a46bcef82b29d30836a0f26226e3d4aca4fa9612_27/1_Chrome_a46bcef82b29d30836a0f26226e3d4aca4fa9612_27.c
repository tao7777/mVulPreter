 void ScreenRecorder::AddConnection(
     scoped_refptr<ConnectionToClient> connection) {
   capture_loop_->PostTask(
       FROM_HERE, base::Bind(&ScreenRecorder::DoInvalidateFullScreen, this));
  network_loop_->PostTask(
      FROM_HERE, base::Bind(&ScreenRecorder::DoAddConnection,
                            this, connection));
 }
