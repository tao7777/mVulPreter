void CueTimeline::UpdateActiveCues(double movie_time) {

  if (IgnoreUpdateRequests())
    return;

  HTMLMediaElement& media_element = MediaElement();

  if (media_element.GetDocument().IsDetached())
    return;


  CueList current_cues;

   if (media_element.getReadyState() != HTMLMediaElement::kHaveNothing &&
      media_element.GetWebMediaPlayer())
     current_cues =
         cue_tree_.AllOverlaps(cue_tree_.CreateInterval(movie_time, movie_time));
 
   CueList previous_cues;
   CueList missed_cues;

  previous_cues = currently_active_cues_;

  double last_time = last_update_time_;
  double last_seek_time = media_element.LastSeekTime();

  if (last_time >= 0 && last_seek_time < movie_time) {
    CueList potentially_skipped_cues =
        cue_tree_.AllOverlaps(cue_tree_.CreateInterval(last_time, movie_time));

    for (CueInterval cue : potentially_skipped_cues) {
      if (cue.Low() > std::max(last_seek_time, last_time) &&
          cue.High() < movie_time)
        missed_cues.push_back(cue);
    }
  }

  last_update_time_ = movie_time;

  if (!media_element.seeking() && last_seek_time < last_time)
    media_element.ScheduleTimeupdateEvent(true);

  size_t missed_cues_size = missed_cues.size();
  size_t previous_cues_size = previous_cues.size();

  bool active_set_changed = missed_cues_size;

  for (size_t i = 0; !active_set_changed && i < previous_cues_size; ++i) {
    if (!current_cues.Contains(previous_cues[i]) &&
        previous_cues[i].Data()->IsActive())
      active_set_changed = true;
  }

  for (CueInterval current_cue : current_cues) {
    if (current_cue.Data()->IsActive())
      current_cue.Data()->UpdatePastAndFutureNodes(movie_time);
    else
      active_set_changed = true;
  }

  if (!active_set_changed)
    return;

  for (size_t i = 0; !media_element.paused() && i < previous_cues_size; ++i) {
    if (previous_cues[i].Data()->pauseOnExit() &&
        previous_cues[i].Data()->IsActive() &&
        !current_cues.Contains(previous_cues[i]))
      media_element.pause();
  }

  for (size_t i = 0; !media_element.paused() && i < missed_cues_size; ++i) {
    if (missed_cues[i].Data()->pauseOnExit())
      media_element.pause();
  }

  HeapVector<std::pair<double, Member<TextTrackCue>>> event_tasks;

  HeapVector<Member<TextTrack>> affected_tracks;

  for (const auto& missed_cue : missed_cues) {
    event_tasks.push_back(
        std::make_pair(missed_cue.Data()->startTime(), missed_cue.Data()));


    if (missed_cue.Data()->startTime() < missed_cue.Data()->endTime()) {
      event_tasks.push_back(
          std::make_pair(missed_cue.Data()->endTime(), missed_cue.Data()));
    }
  }

  for (const auto& previous_cue : previous_cues) {
    if (!current_cues.Contains(previous_cue)) {
      event_tasks.push_back(
          std::make_pair(previous_cue.Data()->endTime(), previous_cue.Data()));
    }
  }

  for (const auto& current_cue : current_cues) {
    if (!previous_cues.Contains(current_cue)) {
      event_tasks.push_back(
          std::make_pair(current_cue.Data()->startTime(), current_cue.Data()));
    }
  }

  NonCopyingSort(event_tasks.begin(), event_tasks.end(), EventTimeCueCompare);

  for (const auto& task : event_tasks) {
    if (!affected_tracks.Contains(task.second->track()))
      affected_tracks.push_back(task.second->track());


    if (task.second->startTime() >= task.second->endTime()) {
      media_element.ScheduleEvent(
          CreateEventWithTarget(EventTypeNames::enter, task.second.Get()));
      media_element.ScheduleEvent(
          CreateEventWithTarget(EventTypeNames::exit, task.second.Get()));
    } else {
      bool is_enter_event = task.first == task.second->startTime();
      AtomicString event_name =
          is_enter_event ? EventTypeNames::enter : EventTypeNames::exit;
      media_element.ScheduleEvent(
          CreateEventWithTarget(event_name, task.second.Get()));
    }
  }

  NonCopyingSort(affected_tracks.begin(), affected_tracks.end(),
                 TrackIndexCompare);

  for (const auto& track : affected_tracks) {
    media_element.ScheduleEvent(
        CreateEventWithTarget(EventTypeNames::cuechange, track.Get()));

    if (track->TrackType() == TextTrack::kTrackElement) {
      HTMLTrackElement* track_element =
          ToLoadableTextTrack(track.Get())->TrackElement();
      DCHECK(track_element);
      media_element.ScheduleEvent(
          CreateEventWithTarget(EventTypeNames::cuechange, track_element));
    }
  }

  for (const auto& cue : current_cues)
    cue.Data()->SetIsActive(true);

  for (const auto& previous_cue : previous_cues) {
    if (!current_cues.Contains(previous_cue)) {
      TextTrackCue* cue = previous_cue.Data();
      cue->SetIsActive(false);
      cue->RemoveDisplayTree();
    }
  }

  currently_active_cues_ = current_cues;
  media_element.UpdateTextTrackDisplay();
}
