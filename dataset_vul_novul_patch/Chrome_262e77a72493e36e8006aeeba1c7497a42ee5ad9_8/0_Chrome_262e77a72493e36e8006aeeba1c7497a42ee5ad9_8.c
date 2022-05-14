 void VRDisplay::cancelAnimationFrame(int id) {
  DVLOG(2) << __FUNCTION__;
   if (!scripted_animation_controller_)
     return;
   scripted_animation_controller_->CancelCallback(id);
 }
