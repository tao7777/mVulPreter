  bool TryAddAxesButton(
  bool TryAddAxesOrTriggerButton(
       vr::EVRButtonId button_id,
       AxesRequirement requirement = AxesRequirement::kOptional) {
     if (!IsInAxesData(button_id))
       return false;
 
    bool require_axes = (requirement == AxesRequirement::kRequireBoth);
     if (require_axes && !axes_data_[button_id].has_both_axes)
       return false;
 
    AddButton(axes_data_[button_id]);
    used_axes_.insert(button_id);

     return true;
   }
