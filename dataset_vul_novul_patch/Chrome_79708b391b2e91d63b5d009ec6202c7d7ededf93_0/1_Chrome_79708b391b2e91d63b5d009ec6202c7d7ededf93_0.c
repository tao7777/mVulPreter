void TestGamepadMinimumDataImpl(WebXrVrBrowserTestBase* t) {
  WebXrControllerInputMock my_mock;

  unsigned int controller_index =
      my_mock.CreateAndConnectMinimalGamepad(t->GetPrimaryAxisType());

  t->LoadUrlAndAwaitInitialization(
       t->GetFileUrlForHtmlTestFile("test_webxr_gamepad_support"));
   t->EnterSessionWithUserGestureOrFail();
 
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
