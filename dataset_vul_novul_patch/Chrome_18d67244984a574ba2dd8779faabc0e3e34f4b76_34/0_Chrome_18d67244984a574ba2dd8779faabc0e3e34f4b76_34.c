void RenderWidgetHostViewAura::UpdateExternalTexture() {
  if (accelerated_compositing_state_changed_)
    accelerated_compositing_state_changed_ = false;

   if (current_surface_ != 0 && host_->is_accelerated_compositing_active()) {
     ui::Texture* container = image_transport_clients_[current_surface_];
     window_->SetExternalTexture(container);
 
     if (!container) {
       resize_locks_.clear();
    } else {
      ResizeLockList::iterator it = resize_locks_.begin();
      while (it != resize_locks_.end()) {
        gfx::Size container_size = ConvertSizeToDIP(this,
            container->size());
        if ((*it)->expected_size() == container_size)
          break;
        ++it;
      }
      if (it != resize_locks_.end()) {
        ++it;
        ui::Compositor* compositor = GetCompositor();
        if (compositor) {
          locks_pending_commit_.insert(
              locks_pending_commit_.begin(), resize_locks_.begin(), it);
          for (ResizeLockList::iterator it2 = resize_locks_.begin();
              it2 !=it; ++it2) {
            it2->get()->UnlockCompositor();
          }
          if (!compositor->HasObserver(this))
            compositor->AddObserver(this);
        }
        resize_locks_.erase(resize_locks_.begin(), it);
      }
     }
   } else {
     window_->SetExternalTexture(NULL);
     resize_locks_.clear();
   }
 }
