 void TextTrackCue::setStartTime(double value) {
  if (start_time_ == value)
     return;
 
   CueWillChange();
  start_time_ = value;
  CueDidChange(kCueMutationAffectsOrder);
 }
