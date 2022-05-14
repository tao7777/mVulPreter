  bool TryAddNextUnusedAxesButton() {
     for (const auto& axes_data_pair : axes_data_) {
       vr::EVRButtonId button_id = axes_data_pair.first;
       if (IsUsed(button_id))
         continue;
 
      if (TryAddAxesButton(button_id, AxesRequirement::kRequired))
         return true;
     }
 
    return false;
  }
