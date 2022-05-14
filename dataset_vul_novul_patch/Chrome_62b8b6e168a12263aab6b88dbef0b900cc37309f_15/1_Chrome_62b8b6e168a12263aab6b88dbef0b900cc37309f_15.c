void PartialMagnificationController::SetScale(float scale) {
  if (!is_enabled_)
     return;
 
  scale_ = scale;
 
  if (IsPartialMagnified()) {
    CreateMagnifierWindow();
  } else {
     CloseMagnifierWindow();
   }
 }
