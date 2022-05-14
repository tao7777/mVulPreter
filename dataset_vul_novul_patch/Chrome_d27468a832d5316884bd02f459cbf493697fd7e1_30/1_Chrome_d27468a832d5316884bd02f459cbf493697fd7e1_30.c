 bool AXTableCell::isColumnHeaderCell() const {
   const AtomicString& scope = getAttribute(scopeAttr);
  return equalIgnoringCase(scope, "col") ||
         equalIgnoringCase(scope, "colgroup");
 }
