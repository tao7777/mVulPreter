void MediaElementAudioSourceHandler::OnCurrentSrcChanged(
    const KURL& current_src) {
  DCHECK(IsMainThread());
  Locker<MediaElementAudioSourceHandler> locker(*this);
 
  passes_current_src_cors_access_check_ =
      PassesCurrentSrcCORSAccessCheck(current_src);
 
  maybe_print_cors_message_ = !passes_current_src_cors_access_check_;
  current_src_string_ = current_src.GetString();
}
