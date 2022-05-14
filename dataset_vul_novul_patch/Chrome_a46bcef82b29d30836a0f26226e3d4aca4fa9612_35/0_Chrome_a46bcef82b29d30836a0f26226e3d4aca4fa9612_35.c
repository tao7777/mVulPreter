 void JingleSessionManager::Close() {
   DCHECK(CalledOnValidThread());
 
   if (!closed_) {
     cricket_session_manager_->RemoveClient(kChromotingXmlNamespace);
     jingle_signaling_connector_.reset();
     closed_ = true;
   }
 }
