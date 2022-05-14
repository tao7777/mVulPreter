 bool AXTableCell::isColumnHeaderCell() const {
   const AtomicString& scope = getAttribute(scopeAttr);
  return equalIgnoringASCIICase(scope, "col") ||
         equalIgnoringASCIICase(scope, "colgroup");
 }
