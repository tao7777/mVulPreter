void DateTimeFieldElement::updateVisibleValue(EventBehavior eventBehavior)
{
    Text* const textNode = toText(firstChild());
    const String newVisibleValue = visibleValue();
    ASSERT(newVisibleValue.length() > 0);

    if (textNode->wholeText() == newVisibleValue)
         return;
 
     textNode->replaceWholeText(newVisibleValue, ASSERT_NO_EXCEPTION);
    setAttribute(aria_valuetextAttr, hasValue() ? newVisibleValue : AXDateTimeFieldEmptyValueText());
    setAttribute(aria_valuenowAttr, newVisibleValue);
 
     if (eventBehavior == DispatchEvent && m_fieldOwner)
         m_fieldOwner->fieldValueChanged();
 }
