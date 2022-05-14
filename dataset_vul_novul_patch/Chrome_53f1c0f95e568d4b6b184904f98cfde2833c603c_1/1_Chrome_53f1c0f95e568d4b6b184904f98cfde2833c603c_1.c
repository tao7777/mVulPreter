 void TextTrackCue::setEndTime(double value) {
  if (end_time_ == value || value < 0)
     return;
 
   CueWillChange();
  end_time_ = value;
  CueDidChange(kCueMutationAffectsOrder);
}
