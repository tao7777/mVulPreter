void BaseMultipleFieldsDateAndTimeInputType::createShadowSubtree()
 {
     ASSERT(element()->shadow());
 
    // Element must not be attached here, because if it was attached
    // DateTimeEditElement::customStyleForRenderer() is called in appendChild()
    // before the field wrapper element is created.
    ASSERT(!element()->attached());

     Document* document = element()->document();
     ContainerNode* container = element()->userAgentShadowRoot();
 
    RefPtr<DateTimeEditElement> dateTimeEditElement(DateTimeEditElement::create(document, *this));
    m_dateTimeEditElement = dateTimeEditElement.get();
    container->appendChild(m_dateTimeEditElement);
    updateInnerTextValue();

    RefPtr<ClearButtonElement> clearButton = ClearButtonElement::create(document, *this);
    m_clearButton = clearButton.get();
    container->appendChild(clearButton);

    RefPtr<SpinButtonElement> spinButton = SpinButtonElement::create(document, *this);
    m_spinButtonElement = spinButton.get();
    container->appendChild(spinButton);

    bool shouldAddPickerIndicator = false;
    if (InputType::themeSupportsDataListUI(this))
        shouldAddPickerIndicator = true;
    RefPtr<RenderTheme> theme = document->page() ? document->page()->theme() : RenderTheme::defaultTheme();
    if (theme->supportsCalendarPicker(formControlType())) {
        shouldAddPickerIndicator = true;
        m_pickerIndicatorIsAlwaysVisible = true;
    }
    if (shouldAddPickerIndicator) {
        RefPtr<PickerIndicatorElement> pickerElement = PickerIndicatorElement::create(document, *this);
        m_pickerIndicatorElement = pickerElement.get();
        container->appendChild(m_pickerIndicatorElement);
        m_pickerIndicatorIsVisible = true;
        updatePickerIndicatorVisibility();
    }
}
