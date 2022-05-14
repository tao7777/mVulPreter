 bool AXARIAGridCell::isAriaRowHeader() const {
   const AtomicString& role = getAttribute(HTMLNames::roleAttr);
  return equalIgnoringCase(role, "rowheader");
 }
