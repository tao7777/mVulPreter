double AudioHandler::VolumeDbToPercent(double volume_db) {
  if (volume_db < kMinVolumeDb)
     return 0;
  return 100.0 * pow((volume_db - kMinVolumeDb) /
      (kMaxVolumeDb - kMinVolumeDb), 1/kVolumeBias);
 }
