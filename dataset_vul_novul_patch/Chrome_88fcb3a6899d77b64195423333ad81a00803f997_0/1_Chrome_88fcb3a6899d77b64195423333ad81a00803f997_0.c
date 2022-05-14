void HTMLFormElement::ScheduleFormSubmission(FormSubmission* submission) {
  DCHECK(submission->Method() == FormSubmission::kPostMethod ||
         submission->Method() == FormSubmission::kGetMethod);
  DCHECK(submission->Data());
  DCHECK(submission->Form());
  if (submission->Action().IsEmpty())
    return;
  if (GetDocument().IsSandboxed(kSandboxForms)) {
    GetDocument().AddConsoleMessage(ConsoleMessage::Create(
        kSecurityMessageSource, kErrorMessageLevel,
        "Blocked form submission to '" + submission->Action().ElidedString() +
            "' because the form's frame is sandboxed and the 'allow-forms' "
            "permission is not set."));
    return;
  }

  if (!GetDocument().GetContentSecurityPolicy()->AllowFormAction(
          submission->Action())) {
    return;
  }

  if (submission->Action().ProtocolIsJavaScript()) {
    GetDocument()
        .GetFrame()
        ->GetScriptController()
        .ExecuteScriptIfJavaScriptURL(submission->Action(), this);
    return;
  }

  Frame* target_frame = GetDocument().GetFrame()->FindFrameForNavigation(
      submission->Target(), *GetDocument().GetFrame(),
      submission->RequestURL());
  if (!target_frame) {
    target_frame = GetDocument().GetFrame();
  } else {
    submission->ClearTarget();
  }
  if (!target_frame->GetPage())
    return;

  UseCounter::Count(GetDocument(), WebFeature::kFormsSubmitted);
  if (MixedContentChecker::IsMixedFormAction(GetDocument().GetFrame(),
                                             submission->Action())) {
    UseCounter::Count(GetDocument().GetFrame(),
                      WebFeature::kMixedContentFormsSubmitted);
  }

  if (target_frame->IsLocalFrame()) {
    ToLocalFrame(target_frame)
        ->GetNavigationScheduler()
        .ScheduleFormSubmission(&GetDocument(), submission);
   } else {
     FrameLoadRequest frame_load_request =
         submission->CreateFrameLoadRequest(&GetDocument());
     ToRemoteFrame(target_frame)->Navigate(frame_load_request);
   }
 }
