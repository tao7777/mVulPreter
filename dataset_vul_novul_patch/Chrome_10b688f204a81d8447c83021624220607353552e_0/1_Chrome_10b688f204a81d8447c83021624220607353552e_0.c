 bool UserActivityDetector::PreHandleMouseEvent(aura::Window* target,
                                                aura::MouseEvent* event) {
  MaybeNotify();
   return false;
 }
