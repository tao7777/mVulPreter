void AudioRendererHost::OnSetVolume(const IPC::Message& msg, int stream_id,
                                    double volume) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  AudioEntry* entry = LookupById(msg.routing_id(), stream_id);
  if (!entry) {
    SendErrorMessage(msg.routing_id(), stream_id);
    return;
  }
 
   if (volume < 0 || volume > 1.0)
    return;
   entry->controller->SetVolume(volume);
 }
