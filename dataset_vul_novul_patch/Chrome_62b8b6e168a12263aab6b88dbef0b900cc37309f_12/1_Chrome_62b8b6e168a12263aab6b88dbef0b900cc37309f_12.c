PartialMagnificationController::PartialMagnificationController()
    : is_enabled_(false),
      scale_(kNonPartialMagnifiedScale),
      zoom_widget_(NULL) {
   Shell::GetInstance()->AddPreTargetHandler(this);
 }
