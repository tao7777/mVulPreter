 void DelegatedFrameHost::ClearDelegatedFrame() {
  // Ensure that we are able to swap in a new blank frame to replace any old
  // content. This will result in a white flash if we switch back to this
  // content.
  // https://crbug.com/739621
  released_front_lock_.reset();
   EvictDelegatedFrame();
 }
