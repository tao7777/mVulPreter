 bool AXARIAGridCell::isAriaColumnHeader() const {
   const AtomicString& role = getAttribute(HTMLNames::roleAttr);
  return equalIgnoringASCIICase(role, "columnheader");
 }
