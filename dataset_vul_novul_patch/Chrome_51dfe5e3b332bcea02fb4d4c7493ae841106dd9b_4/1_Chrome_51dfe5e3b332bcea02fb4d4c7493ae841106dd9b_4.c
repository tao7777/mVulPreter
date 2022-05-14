 void AudioHandler::SetVolumePercent(double volume_percent) {
   if (!VerifyMixerConnection())
     return;
   DCHECK(volume_percent >= 0.0);
 
   double vol_db;
   if (volume_percent <= 0)
    vol_db = kSilenceDb;
   else
     vol_db = PercentToVolumeDb(volume_percent);
 
  mixer_->SetVolumeDb(vol_db);
}
