void TestGamepadMinimumDataImpl(WebXrVrBrowserTestBase* t) {
  WebXrControllerInputMock my_mock;

  unsigned int controller_index =
      my_mock.CreateAndConnectMinimalGamepad(t->GetPrimaryAxisType());

  t->LoadUrlAndAwaitInitialization(
       t->GetFileUrlForHtmlTestFile("test_webxr_gamepad_support"));
   t->EnterSessionWithUserGestureOrFail();
 
  // We only actually connect the data for the two buttons, but WMR expects
  // the WMR controller (which has all of the required and optional buttons)
  // and so adds dummy/placeholder buttons regardless of what data we send up.
  std::string button_count = "2";
  if (t->GetRuntimeType() == XrBrowserTestBase::RuntimeType::RUNTIME_WMR)
    button_count = "4";

  t->PollJavaScriptBooleanOrFail("isButtonCountEqualTo(" + button_count + ")",
                                 WebXrVrBrowserTestBase::kPollTimeoutShort);

   my_mock.TogglePrimaryTrigger(controller_index);
  my_mock.SetAxes(controller_index, device::XrButtonId::kAxisPrimary, 0.5,
                  -0.5);
  my_mock.ToggleButtonTouches(controller_index,
                              device::XrButtonId::kAxisPrimary);

  t->PollJavaScriptBooleanOrFail("isMappingEqualTo('xr-standard')",
                                 WebXrVrBrowserTestBase::kPollTimeoutShort);
  t->PollJavaScriptBooleanOrFail("isButtonPressedEqualTo(0, true)",
                                 WebXrVrBrowserTestBase::kPollTimeoutShort);
  t->PollJavaScriptBooleanOrFail("areAxesValuesEqualTo(0, 0.5, -0.5)",
                                 WebXrVrBrowserTestBase::kPollTimeoutShort);
  t->RunJavaScriptOrFail("done()");
  t->EndTest();
}
