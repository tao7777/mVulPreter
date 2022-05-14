  bool TryAddNextUnusedAxesButton() {
  bool TryAddNextUnusedButtonWithAxes() {
     for (const auto& axes_data_pair : axes_data_) {
       vr::EVRButtonId button_id = axes_data_pair.first;
       if (IsUsed(button_id))
         continue;
 
      if (TryAddAxesOrTriggerButton(button_id, AxesRequirement::kRequireBoth))
         return true;
     }
 
    return false;
  }
