void Document::DispatchUnloadEvents() {
  PluginScriptForbiddenScope forbid_plugin_destructor_scripting;
  if (parser_)
    parser_->StopParsing();

  if (load_event_progress_ == kLoadEventNotRun)
    return;

  if (load_event_progress_ <= kUnloadEventInProgress) {
    Element* current_focused_element = FocusedElement();
    if (auto* input = ToHTMLInputElementOrNull(current_focused_element))
      input->EndEditing();
    if (load_event_progress_ < kPageHideInProgress) {
      load_event_progress_ = kPageHideInProgress;
      if (LocalDOMWindow* window = domWindow()) {
        const TimeTicks pagehide_event_start = CurrentTimeTicks();
        window->DispatchEvent(
            PageTransitionEvent::Create(EventTypeNames::pagehide, false), this);
        const TimeTicks pagehide_event_end = CurrentTimeTicks();
        DEFINE_STATIC_LOCAL(
            CustomCountHistogram, pagehide_histogram,
            ("DocumentEventTiming.PageHideDuration", 0, 10000000, 50));
        pagehide_histogram.Count(
            (pagehide_event_end - pagehide_event_start).InMicroseconds());
      }
      if (!frame_)
        return;

      mojom::PageVisibilityState visibility_state = GetPageVisibilityState();
      load_event_progress_ = kUnloadVisibilityChangeInProgress;
      if (visibility_state != mojom::PageVisibilityState::kHidden) {
        const TimeTicks pagevisibility_hidden_event_start = CurrentTimeTicks();
        DispatchEvent(Event::CreateBubble(EventTypeNames::visibilitychange));
        const TimeTicks pagevisibility_hidden_event_end = CurrentTimeTicks();
        DEFINE_STATIC_LOCAL(CustomCountHistogram, pagevisibility_histogram,
                            ("DocumentEventTiming.PageVibilityHiddenDuration",
                             0, 10000000, 50));
        pagevisibility_histogram.Count((pagevisibility_hidden_event_end -
                                        pagevisibility_hidden_event_start)
                                           .InMicroseconds());
        DispatchEvent(
            Event::CreateBubble(EventTypeNames::webkitvisibilitychange));
      }
      if (!frame_)
        return;

      frame_->Loader().SaveScrollAnchor();

      DocumentLoader* document_loader =
          frame_->Loader().GetProvisionalDocumentLoader();
      load_event_progress_ = kUnloadEventInProgress;
      Event* unload_event(Event::Create(EventTypeNames::unload));
      if (document_loader &&
          document_loader->GetTiming().UnloadEventStart().is_null() &&
          document_loader->GetTiming().UnloadEventEnd().is_null()) {
        DocumentLoadTiming& timing = document_loader->GetTiming();
        DCHECK(!timing.NavigationStart().is_null());
        const TimeTicks unload_event_start = CurrentTimeTicks();
        timing.MarkUnloadEventStart(unload_event_start);
        frame_->DomWindow()->DispatchEvent(unload_event, this);
        const TimeTicks unload_event_end = CurrentTimeTicks();
        DEFINE_STATIC_LOCAL(
            CustomCountHistogram, unload_histogram,
            ("DocumentEventTiming.UnloadDuration", 0, 10000000, 50));
        unload_histogram.Count(
            (unload_event_end - unload_event_start).InMicroseconds());
        timing.MarkUnloadEventEnd(unload_event_end);
      } else {
        frame_->DomWindow()->DispatchEvent(unload_event, frame_->GetDocument());
      }
    }
    load_event_progress_ = kUnloadEventHandled;
  }

  if (!frame_)
    return;

   bool keep_event_listeners =
       frame_->Loader().GetProvisionalDocumentLoader() &&
       frame_->ShouldReuseDefaultView(
          frame_->Loader().GetProvisionalDocumentLoader()->Url());
   if (!keep_event_listeners)
     RemoveAllEventListenersRecursively();
 }
