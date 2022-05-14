void DateTimeFieldElement::updateVisibleValue(EventBehavior eventBehavior)
{
    Text* const textNode = toText(firstChild());
    const String newVisibleValue = visibleValue();
    ASSERT(newVisibleValue.length() > 0);

    if (textNode->wholeText() == newVisibleValue)
         return;
 
     textNode->replaceWholeText(newVisibleValue, ASSERT_NO_EXCEPTION);
    if (hasValue()) {
        setAttribute(aria_valuetextAttr, newVisibleValue);
        setAttribute(aria_valuenowAttr, String::number(valueForARIAValueNow()));
    } else {
        setAttribute(aria_valuetextAttr, AXDateTimeFieldEmptyValueText());
        removeAttribute(aria_valuenowAttr);
    }
 
     if (eventBehavior == DispatchEvent && m_fieldOwner)
         m_fieldOwner->fieldValueChanged();
 }
