void HandleMagnifyScreen(int delta_index) {
  if (Shell::GetInstance()->magnification_controller()->IsEnabled()) {
    float scale = Shell::GetInstance()->magnification_controller()->GetScale();
    int scale_index =
        std::floor(std::log(scale) / std::log(kMagnificationScaleFactor) + 0.5);

    int new_scale_index = std::max(0, std::min(8, scale_index + delta_index));
 
     Shell::GetInstance()->magnification_controller()->SetScale(
         std::pow(kMagnificationScaleFactor, new_scale_index), true);
  } else if (Shell::GetInstance()
                 ->partial_magnification_controller()
                 ->is_enabled()) {
    float scale = delta_index > 0 ? kDefaultPartialMagnifiedScale : 1;
    Shell::GetInstance()->partial_magnification_controller()->SetScale(scale);
   }
 }
