ClassicScript* ClassicPendingScript::GetSource(const KURL& document_url,
                                               bool& error_occurred) const {
  CheckState();
  DCHECK(IsReady());

  error_occurred = ErrorOccurred();
  if (!is_external_) {
    ScriptSourceCode source_code(
        GetElement()->TextFromChildren(), source_location_type_,
        nullptr /* cache_handler */, document_url, StartingPosition());
    return ClassicScript::Create(source_code, base_url_for_inline_script_,
                                 options_, kSharableCrossOrigin);
  }

  DCHECK(GetResource()->IsLoaded());
  ScriptResource* resource = ToScriptResource(GetResource());
  bool streamer_ready = (ready_state_ == kReady) && streamer_ &&
                        !streamer_->StreamingSuppressed();
  ScriptSourceCode source_code(streamer_ready ? streamer_ : nullptr, resource);
   const KURL& base_url = source_code.Url();
  return ClassicScript::Create(
      source_code, base_url, options_,
      resource->CalculateAccessControlStatus(
          GetElement()->GetDocument().GetSecurityOrigin()));
 }
