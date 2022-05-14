void BaseMultipleFieldsDateAndTimeInputType::destroyShadowSubtree()
{
    ASSERT(!m_isDestroyingShadowSubtree);
    m_isDestroyingShadowSubtree = true;
    if (SpinButtonElement* element = spinButtonElement())
        element->removeSpinButtonOwner();
    if (ClearButtonElement* element = clearButtonElement())
        element->removeClearButtonOwner();
    if (DateTimeEditElement* element = dateTimeEditElement())
        element->removeEditControlOwner();
    if (PickerIndicatorElement* element = pickerIndicatorElement())
        element->removePickerIndicatorOwner();
 
    if (containsFocusedShadowElement())
         element()->focus();
 
     BaseDateAndTimeInputType::destroyShadowSubtree();
    m_isDestroyingShadowSubtree = false;
}
