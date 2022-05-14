 AudioHandler::AudioHandler()
     : connected_(false),
      reconnect_tries_(0),
      max_volume_db_(kMaxVolumeDb),
      min_volume_db_(kMinVolumeDb),
      mixer_type_(MIXER_TYPE_PULSEAUDIO) {

  // Start trying to connect to mixers asychronously, starting with the current
  // mixer_type_.  If the connection fails, another TryToConnect() for the next
  // mixer will be posted at that time.
  TryToConnect(true);
 }
