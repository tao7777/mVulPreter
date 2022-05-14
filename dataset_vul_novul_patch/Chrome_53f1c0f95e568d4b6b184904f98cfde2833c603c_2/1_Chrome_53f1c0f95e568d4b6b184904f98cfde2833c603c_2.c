 void TextTrackCue::setStartTime(double value) {
  if (start_time_ == value || value < 0)
     return;
 
   CueWillChange();
  start_time_ = value;
  CueDidChange(kCueMutationAffectsOrder);
 }
