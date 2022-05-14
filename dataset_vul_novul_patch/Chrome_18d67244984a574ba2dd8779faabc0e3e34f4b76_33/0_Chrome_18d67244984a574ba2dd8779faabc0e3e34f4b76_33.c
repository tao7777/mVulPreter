bool RenderWidgetHostViewAura::ShouldFastACK(uint64 surface_id) {
bool RenderWidgetHostViewAura::ShouldSkipFrame(const gfx::Size& size) {
   if (can_lock_compositor_ == NO_PENDING_RENDERER_FRAME ||
       can_lock_compositor_ == NO_PENDING_COMMIT ||
       resize_locks_.empty())
     return false;
 
  gfx::Size container_size = ConvertSizeToDIP(this, size);
   ResizeLockList::iterator it = resize_locks_.begin();
   while (it != resize_locks_.end()) {
     if ((*it)->expected_size() == container_size)
      break;
    ++it;
  }

  return it == resize_locks_.end() || ++it != resize_locks_.end();
}
