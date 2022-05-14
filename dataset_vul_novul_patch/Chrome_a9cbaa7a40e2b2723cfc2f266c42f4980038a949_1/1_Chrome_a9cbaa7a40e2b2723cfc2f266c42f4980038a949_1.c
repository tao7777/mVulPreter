bool WebMediaPlayerMS::HasSingleSecurityOrigin() const {
  DCHECK(thread_checker_.CalledOnValidThread());
  return true;
}
