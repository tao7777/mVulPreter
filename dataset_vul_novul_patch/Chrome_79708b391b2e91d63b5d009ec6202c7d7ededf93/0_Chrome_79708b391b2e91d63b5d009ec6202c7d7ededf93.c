 void TestGamepadCompleteDataImpl(WebXrVrBrowserTestBase* t) {
   WebXrControllerInputMock my_mock;
 
  // Create a controller that supports all reserved buttons.
   uint64_t supported_buttons =
       device::XrButtonMaskFromId(device::XrButtonId::kAxisTrigger) |
       device::XrButtonMaskFromId(device::XrButtonId::kAxisPrimary) |
      device::XrButtonMaskFromId(device::XrButtonId::kAxisSecondary) |
      device::XrButtonMaskFromId(device::XrButtonId::kGrip);

  std::map<device::XrButtonId, unsigned int> axis_types = {
      {device::XrButtonId::kAxisPrimary, t->GetPrimaryAxisType()},
      {device::XrButtonId::kAxisTrigger, device::XrAxisType::kTrigger},
      {device::XrButtonId::kAxisSecondary, t->GetSecondaryAxisType()},
  };

  unsigned int controller_index = my_mock.CreateAndConnectController(
      device::ControllerRole::kControllerRoleRight, axis_types,
      supported_buttons);

  t->LoadUrlAndAwaitInitialization(
      t->GetFileUrlForHtmlTestFile("test_webxr_gamepad_support"));
  t->EnterSessionWithUserGestureOrFail();

  my_mock.SetAxes(controller_index, device::XrButtonId::kAxisSecondary, 0.25,
                  -0.25);

  my_mock.ToggleButtonTouches(
      controller_index,
      device::XrButtonMaskFromId(device::XrButtonId::kAxisSecondary));

  my_mock.ToggleButtons(controller_index,
                        device::XrButtonMaskFromId(device::XrButtonId::kGrip));

   t->PollJavaScriptBooleanOrFail("isMappingEqualTo('xr-standard')",
                                  WebVrBrowserTestBase::kPollTimeoutShort);
 
  // Controller should have all required and optional xr-standard buttons
  t->PollJavaScriptBooleanOrFail("isButtonCountEqualTo(4)",
                                 WebXrVrBrowserTestBase::kPollTimeoutShort);

   t->PollJavaScriptBooleanOrFail("areAxesValuesEqualTo(1, 0.25, -0.25)",
                                  WebVrBrowserTestBase::kPollTimeoutShort);

  t->PollJavaScriptBooleanOrFail("isButtonPressedEqualTo(2, true)",
                                 WebVrBrowserTestBase::kPollTimeoutShort);

  t->PollJavaScriptBooleanOrFail("isButtonPressedEqualTo(3, false)",
                                 WebVrBrowserTestBase::kPollTimeoutShort);
  t->PollJavaScriptBooleanOrFail("isButtonTouchedEqualTo(3, true)",
                                 WebVrBrowserTestBase::kPollTimeoutShort);

  t->RunJavaScriptOrFail("done()");
  t->EndTest();
}
