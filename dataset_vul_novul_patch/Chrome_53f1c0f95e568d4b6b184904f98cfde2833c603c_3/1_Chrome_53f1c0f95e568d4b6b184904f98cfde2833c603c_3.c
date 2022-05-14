 bool TextTrackCueList::Add(TextTrackCue* cue) {
  DCHECK_GE(cue->startTime(), 0);
  DCHECK_GE(cue->endTime(), 0);
   size_t index = FindInsertionIndex(cue);

  if (!list_.IsEmpty() && (index > 0) && (list_[index - 1].Get() == cue))
    return false;

  list_.insert(index, cue);
  InvalidateCueIndex(index);
  return true;
}
