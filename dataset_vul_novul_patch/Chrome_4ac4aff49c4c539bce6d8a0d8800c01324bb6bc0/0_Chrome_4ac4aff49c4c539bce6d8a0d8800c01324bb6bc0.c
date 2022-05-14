void HTMLFormElement::scheduleFormSubmission(FormSubmission* submission) {
  DCHECK(submission->method() == FormSubmission::PostMethod ||
         submission->method() == FormSubmission::GetMethod);
  DCHECK(submission->data());
  DCHECK(submission->form());
  if (submission->action().isEmpty())
    return;
  if (document().isSandboxed(SandboxForms)) {
    document().addConsoleMessage(ConsoleMessage::create(
        SecurityMessageSource, ErrorMessageLevel,
        "Blocked form submission to '" + submission->action().elidedString() +
            "' because the form's frame is sandboxed and the 'allow-forms' "
            "permission is not set."));
     return;
   }
 
  if (!document().contentSecurityPolicy()->allowFormAction(
          submission->action())) {
    return;
  }

   if (protocolIsJavaScript(submission->action())) {
     document().frame()->script().executeScriptIfJavaScriptURL(
         submission->action(), this);
     return;
  }

  Frame* targetFrame = document().frame()->findFrameForNavigation(
      submission->target(), *document().frame());
  if (!targetFrame) {
    if (!LocalDOMWindow::allowPopUp(*document().frame()) &&
        !UserGestureIndicator::utilizeUserGesture())
      return;
    targetFrame = document().frame();
  } else {
    submission->clearTarget();
  }
  if (!targetFrame->host())
    return;

  UseCounter::count(document(), UseCounter::FormsSubmitted);
  if (MixedContentChecker::isMixedFormAction(document().frame(),
                                             submission->action()))
    UseCounter::count(document().frame(),
                      UseCounter::MixedContentFormsSubmitted);

  if (targetFrame->isLocalFrame()) {
    toLocalFrame(targetFrame)
        ->navigationScheduler()
        .scheduleFormSubmission(&document(), submission);
  } else {
    FrameLoadRequest frameLoadRequest =
        submission->createFrameLoadRequest(&document());
    toRemoteFrame(targetFrame)->navigate(frameLoadRequest);
  }
}
