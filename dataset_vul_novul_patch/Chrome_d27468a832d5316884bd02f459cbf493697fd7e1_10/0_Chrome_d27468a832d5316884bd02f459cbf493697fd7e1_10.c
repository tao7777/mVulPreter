 InvalidState AXNodeObject::getInvalidState() const {
   const AtomicString& attributeValue =
       getAOMPropertyOrARIAAttribute(AOMStringProperty::kInvalid);
  if (equalIgnoringASCIICase(attributeValue, "false"))
     return InvalidStateFalse;
  if (equalIgnoringASCIICase(attributeValue, "true"))
     return InvalidStateTrue;
  if (equalIgnoringASCIICase(attributeValue, "spelling"))
     return InvalidStateSpelling;
  if (equalIgnoringASCIICase(attributeValue, "grammar"))
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
