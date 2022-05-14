 void JingleSessionManager::Close() {
   DCHECK(CalledOnValidThread());
 
  DCHECK(sessions_.empty());
   if (!closed_) {
     cricket_session_manager_->RemoveClient(kChromotingXmlNamespace);
     jingle_signaling_connector_.reset();
    cricket_session_manager_.reset();
     closed_ = true;
   }
 }
