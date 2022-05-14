base::Optional<Gamepad> OpenVRGamepadHelper::GetXRGamepad(
    vr::IVRSystem* vr_system,
    uint32_t controller_id,
    vr::VRControllerState_t controller_state,
    device::mojom::XRHandedness handedness) {
   OpenVRGamepadBuilder builder(vr_system, controller_id, controller_state,
                                handedness);
 
  if (!builder.TryAddAxesOrTriggerButton(vr::k_EButton_SteamVR_Trigger))
     return base::nullopt;
 
  if (!builder.TryAddNextUnusedButtonWithAxes())
     return base::nullopt;
 
  bool added_placeholder_grip = false;
  if (!builder.TryAddButton(vr::k_EButton_Grip)) {
    added_placeholder_grip = true;
     builder.AddPlaceholderButton();
  }
 
  bool added_placeholder_axes = false;
  if (!builder.TryAddNextUnusedButtonWithAxes()) {
    added_placeholder_axes = true;
     builder.AddPlaceholderButton();
  }
 
  bool added_optional_buttons = false;
   for (const auto& button : kWebXRButtonOrder) {
    added_optional_buttons =
        builder.TryAddButton(button) || added_optional_buttons;
   }
 
  bool added_optional_axes = builder.AddRemainingTriggersAndAxes();

  // If we didn't add any optional buttons, we need to remove our placeholder
  // buttons.
  if (!(added_optional_buttons || added_optional_axes)) {
    // If we didn't add any optional buttons, see if we need to remove the most
    // recent placeholder (the secondary axes).
    // Note that if we added a placeholder axes, the only optional axes that
    // should have been added are triggers, and so we don't need to worry about
    // the order
    if (added_placeholder_axes) {
      builder.RemovePlaceholderButton();

      // Only if the axes button was a placeholder can we remove the grip
      // if it was also a placeholder.
      if (added_placeholder_grip) {
        builder.RemovePlaceholderButton();
      }
    }
  }
 
   return builder.GetGamepad();
 }
