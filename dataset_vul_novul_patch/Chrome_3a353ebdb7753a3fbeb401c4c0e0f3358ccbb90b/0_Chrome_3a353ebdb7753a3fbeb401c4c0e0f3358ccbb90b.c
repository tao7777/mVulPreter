HTMLMediaElement::HTMLMediaElement(const QualifiedName& tag_name,
                                   Document& document)
    : HTMLElement(tag_name, document),
      PausableObject(&document),
      load_timer_(document.GetTaskRunner(TaskType::kInternalMedia),
                  this,
                  &HTMLMediaElement::LoadTimerFired),
      progress_event_timer_(
          document.GetTaskRunner(TaskType::kMediaElementEvent),
          this,
          &HTMLMediaElement::ProgressEventTimerFired),
      playback_progress_timer_(document.GetTaskRunner(TaskType::kInternalMedia),
                               this,
                               &HTMLMediaElement::PlaybackProgressTimerFired),
      audio_tracks_timer_(document.GetTaskRunner(TaskType::kInternalMedia),
                          this,
                          &HTMLMediaElement::AudioTracksTimerFired),
      removed_from_document_timer_(
          document.GetTaskRunner(TaskType::kInternalMedia),
          this,
          &HTMLMediaElement::OnRemovedFromDocumentTimerFired),
      played_time_ranges_(),
      async_event_queue_(EventQueue::Create(GetExecutionContext(),
                                            TaskType::kMediaElementEvent)),
      playback_rate_(1.0f),
      default_playback_rate_(1.0f),
      network_state_(kNetworkEmpty),
      ready_state_(kHaveNothing),
      ready_state_maximum_(kHaveNothing),
      volume_(1.0f),
      last_seek_time_(0),
      previous_progress_time_(std::numeric_limits<double>::max()),
      duration_(std::numeric_limits<double>::quiet_NaN()),
      last_time_update_event_media_time_(
          std::numeric_limits<double>::quiet_NaN()),
      default_playback_start_position_(0),
      load_state_(kWaitingForSource),
      deferred_load_state_(kNotDeferred),
      deferred_load_timer_(document.GetTaskRunner(TaskType::kInternalMedia),
                           this,
                           &HTMLMediaElement::DeferredLoadTimerFired),
      cc_layer_(nullptr),
      display_mode_(kUnknown),
      official_playback_position_(0),
      official_playback_position_needs_update_(true),
      fragment_end_time_(std::numeric_limits<double>::quiet_NaN()),
      pending_action_flags_(0),
      playing_(false),
      should_delay_load_event_(false),
      have_fired_loaded_data_(false),
      can_autoplay_(true),
       muted_(false),
       paused_(true),
       seeking_(false),
      paused_by_context_paused_(false),
       sent_stalled_event_(false),
       ignore_preload_none_(false),
       text_tracks_visible_(false),
      should_perform_automatic_track_selection_(true),
      tracks_are_ready_(true),
      processing_preference_change_(false),
      playing_remotely_(false),
      mostly_filling_viewport_(false),
      was_always_muted_(true),
      audio_tracks_(AudioTrackList::Create(*this)),
      video_tracks_(VideoTrackList::Create(*this)),
      audio_source_node_(nullptr),
      autoplay_policy_(MakeGarbageCollected<AutoplayPolicy>(this)),
      remote_playback_client_(nullptr),
      media_controls_(nullptr),
      controls_list_(HTMLMediaElementControlsList::Create(this)),
      lazy_load_visibility_observer_(nullptr) {
  BLINK_MEDIA_LOG << "HTMLMediaElement(" << (void*)this << ")";

  LocalFrame* frame = document.GetFrame();
  if (frame) {
    remote_playback_client_ =
        frame->Client()->CreateWebRemotePlaybackClient(*this);
  }

  SetHasCustomStyleCallbacks();
  AddElementToDocumentMap(this, &document);

  UseCounter::Count(document, WebFeature::kHTMLMediaElement);
}
