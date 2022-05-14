 bool HTMLFormElement::prepareForSubmission(Event* event)
 {
    RefPtr<HTMLFormElement> protector(this);
     Frame* frame = document().frame();
     if (m_isSubmittingOrPreparingForSubmission || !frame)
         return m_isSubmittingOrPreparingForSubmission;

    m_isSubmittingOrPreparingForSubmission = true;
    m_shouldSubmit = false;

    if (!validateInteractively(event)) {
        m_isSubmittingOrPreparingForSubmission = false;
        return false;
    }

    StringPairVector controlNamesAndValues;
    getTextFieldValues(controlNamesAndValues);
    RefPtr<FormState> formState = FormState::create(this, controlNamesAndValues, &document(), NotSubmittedByJavaScript);
    frame->loader()->client()->dispatchWillSendSubmitEvent(formState.release());

    if (dispatchEvent(Event::createCancelableBubble(eventNames().submitEvent)))
        m_shouldSubmit = true;

    m_isSubmittingOrPreparingForSubmission = false;

    if (m_shouldSubmit)
        submit(event, true, true, NotSubmittedByJavaScript);

    return m_shouldSubmit;
}
