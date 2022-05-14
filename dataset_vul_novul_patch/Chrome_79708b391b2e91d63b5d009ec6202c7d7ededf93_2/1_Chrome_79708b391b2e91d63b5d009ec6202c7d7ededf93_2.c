base::Optional<Gamepad> OpenVRGamepadHelper::GetXRGamepad(
    vr::IVRSystem* vr_system,
    uint32_t controller_id,
    vr::VRControllerState_t controller_state,
    device::mojom::XRHandedness handedness) {
   OpenVRGamepadBuilder builder(vr_system, controller_id, controller_state,
                                handedness);
 
  if (!builder.TryAddAxesButton(vr::k_EButton_SteamVR_Trigger))
     return base::nullopt;
 
  if (!builder.TryAddNextUnusedAxesButton())
     return base::nullopt;
 
  if (!builder.TryAddButton(vr::k_EButton_Grip))
     builder.AddPlaceholderButton();
 
  if (!builder.TryAddNextUnusedAxesButton())
     builder.AddPlaceholderButton();
 
   for (const auto& button : kWebXRButtonOrder) {
    builder.TryAddButton(button);
   }
 
  builder.AddRemainingAxes();
 
   return builder.GetGamepad();
 }
