void AudioHandler::AdjustVolumeByPercent(double adjust_by_percent) {
  if (!VerifyMixerConnection())
    return;

  DVLOG(1) << "Adjusting Volume by " << adjust_by_percent << " percent";

  double volume = mixer_->GetVolumeDb();
  double pct = VolumeDbToPercent(volume);

  if (pct < 0)
    pct = 0;
  pct = pct + adjust_by_percent;
  if (pct > 100.0)
    pct = 100.0;
 
   double new_volume;
   if (pct <= 0.1)
    new_volume = kSilenceDb;
   else
     new_volume = PercentToVolumeDb(pct);
 
  if (new_volume != volume)
    mixer_->SetVolumeDb(new_volume);
}
