double AudioHandler::VolumeDbToPercent(double volume_db) {
double AudioHandler::VolumeDbToPercent(double volume_db) const {
  if (volume_db < min_volume_db_)
     return 0;
  return 100.0 * pow((volume_db - min_volume_db_) /
      (max_volume_db_ - min_volume_db_), 1/kVolumeBias);
 }
