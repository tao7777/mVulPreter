AriaCurrentState AXNodeObject::ariaCurrentState() const {
  const AtomicString& attributeValue =
       getAOMPropertyOrARIAAttribute(AOMStringProperty::kCurrent);
   if (attributeValue.isNull())
     return AriaCurrentStateUndefined;
  if (attributeValue.isEmpty() ||
      equalIgnoringASCIICase(attributeValue, "false"))
     return AriaCurrentStateFalse;
  if (equalIgnoringASCIICase(attributeValue, "true"))
     return AriaCurrentStateTrue;
  if (equalIgnoringASCIICase(attributeValue, "page"))
     return AriaCurrentStatePage;
  if (equalIgnoringASCIICase(attributeValue, "step"))
     return AriaCurrentStateStep;
  if (equalIgnoringASCIICase(attributeValue, "location"))
     return AriaCurrentStateLocation;
  if (equalIgnoringASCIICase(attributeValue, "date"))
     return AriaCurrentStateDate;
  if (equalIgnoringASCIICase(attributeValue, "time"))
     return AriaCurrentStateTime;
   if (!attributeValue.isEmpty())
    return AriaCurrentStateTrue;

  return AXObject::ariaCurrentState();
}
