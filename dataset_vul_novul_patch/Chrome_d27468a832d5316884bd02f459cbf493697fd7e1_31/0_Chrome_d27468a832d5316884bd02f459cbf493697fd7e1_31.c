 bool AXTableCell::isRowHeaderCell() const {
   const AtomicString& scope = getAttribute(scopeAttr);
  return equalIgnoringASCIICase(scope, "row") ||
         equalIgnoringASCIICase(scope, "rowgroup");
 }
