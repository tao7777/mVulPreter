 bool AXARIAGridCell::isAriaColumnHeader() const {
   const AtomicString& role = getAttribute(HTMLNames::roleAttr);
  return equalIgnoringCase(role, "columnheader");
 }
