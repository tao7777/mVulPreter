 bool AXTableCell::isRowHeaderCell() const {
   const AtomicString& scope = getAttribute(scopeAttr);
  return equalIgnoringCase(scope, "row") ||
         equalIgnoringCase(scope, "rowgroup");
 }
