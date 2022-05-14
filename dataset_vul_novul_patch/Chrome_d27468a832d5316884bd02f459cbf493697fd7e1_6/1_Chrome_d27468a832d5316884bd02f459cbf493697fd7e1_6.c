AriaCurrentState AXNodeObject::ariaCurrentState() const {
  const AtomicString& attributeValue =
       getAOMPropertyOrARIAAttribute(AOMStringProperty::kCurrent);
   if (attributeValue.isNull())
     return AriaCurrentStateUndefined;
  if (attributeValue.isEmpty() || equalIgnoringCase(attributeValue, "false"))
     return AriaCurrentStateFalse;
  if (equalIgnoringCase(attributeValue, "true"))
     return AriaCurrentStateTrue;
  if (equalIgnoringCase(attributeValue, "page"))
     return AriaCurrentStatePage;
  if (equalIgnoringCase(attributeValue, "step"))
     return AriaCurrentStateStep;
  if (equalIgnoringCase(attributeValue, "location"))
     return AriaCurrentStateLocation;
  if (equalIgnoringCase(attributeValue, "date"))
     return AriaCurrentStateDate;
  if (equalIgnoringCase(attributeValue, "time"))
     return AriaCurrentStateTime;
   if (!attributeValue.isEmpty())
    return AriaCurrentStateTrue;

  return AXObject::ariaCurrentState();
}
