double AudioHandler::PercentToVolumeDb(double volume_percent) {
   return pow(volume_percent / 100.0, kVolumeBias) *
      (kMaxVolumeDb - kMinVolumeDb) + kMinVolumeDb;
 }
