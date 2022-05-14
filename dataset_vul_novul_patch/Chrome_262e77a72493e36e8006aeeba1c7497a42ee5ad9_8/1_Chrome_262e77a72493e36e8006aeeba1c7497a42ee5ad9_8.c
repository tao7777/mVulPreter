 void VRDisplay::cancelAnimationFrame(int id) {
   if (!scripted_animation_controller_)
     return;
   scripted_animation_controller_->CancelCallback(id);
 }
