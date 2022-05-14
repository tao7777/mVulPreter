 InvalidState AXNodeObject::getInvalidState() const {
   const AtomicString& attributeValue =
       getAOMPropertyOrARIAAttribute(AOMStringProperty::kInvalid);
  if (equalIgnoringCase(attributeValue, "false"))
     return InvalidStateFalse;
  if (equalIgnoringCase(attributeValue, "true"))
     return InvalidStateTrue;
  if (equalIgnoringCase(attributeValue, "spelling"))
     return InvalidStateSpelling;
  if (equalIgnoringCase(attributeValue, "grammar"))
     return InvalidStateGrammar;
   if (!attributeValue.isEmpty())
    return InvalidStateOther;

  if (getNode() && getNode()->isElementNode() &&
      toElement(getNode())->isFormControlElement()) {
    HTMLFormControlElement* element = toHTMLFormControlElement(getNode());
    HeapVector<Member<HTMLFormControlElement>> invalidControls;
    bool isInvalid =
        !element->checkValidity(&invalidControls, CheckValidityDispatchNoEvent);
    return isInvalid ? InvalidStateTrue : InvalidStateFalse;
  }

  return AXObject::getInvalidState();
}
