 bool AXARIAGridCell::isAriaRowHeader() const {
   const AtomicString& role = getAttribute(HTMLNames::roleAttr);
  return equalIgnoringASCIICase(role, "rowheader");
 }
