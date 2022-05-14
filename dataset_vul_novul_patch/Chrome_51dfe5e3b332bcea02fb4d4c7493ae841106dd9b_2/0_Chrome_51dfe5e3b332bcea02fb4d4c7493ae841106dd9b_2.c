double AudioHandler::PercentToVolumeDb(double volume_percent) {
double AudioHandler::PercentToVolumeDb(double volume_percent) const {
   return pow(volume_percent / 100.0, kVolumeBias) *
      (max_volume_db_ - min_volume_db_) + min_volume_db_;
 }
