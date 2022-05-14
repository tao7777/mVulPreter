bool WebMediaPlayerMS::DidPassCORSAccessCheck() const {
  DCHECK(thread_checker_.CalledOnValidThread());
  return true;
}
