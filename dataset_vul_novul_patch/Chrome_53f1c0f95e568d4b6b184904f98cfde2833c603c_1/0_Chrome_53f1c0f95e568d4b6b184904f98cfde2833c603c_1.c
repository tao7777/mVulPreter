 void TextTrackCue::setEndTime(double value) {
  if (end_time_ == value)
     return;
 
   CueWillChange();
  end_time_ = value;
  CueDidChange(kCueMutationAffectsOrder);
}
