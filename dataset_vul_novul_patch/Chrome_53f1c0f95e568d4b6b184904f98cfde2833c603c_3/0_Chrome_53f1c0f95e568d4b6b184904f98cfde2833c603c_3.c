 bool TextTrackCueList::Add(TextTrackCue* cue) {
   size_t index = FindInsertionIndex(cue);

  if (!list_.IsEmpty() && (index > 0) && (list_[index - 1].Get() == cue))
    return false;

  list_.insert(index, cue);
  InvalidateCueIndex(index);
  return true;
}
