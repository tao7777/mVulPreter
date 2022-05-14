SortDirection AXTableCell::getSortDirection() const {
  if (roleValue() != RowHeaderRole && roleValue() != ColumnHeaderRole)
    return SortDirectionUndefined;

  const AtomicString& ariaSort =
       getAOMPropertyOrARIAAttribute(AOMStringProperty::kSort);
   if (ariaSort.isEmpty())
     return SortDirectionUndefined;
  if (equalIgnoringASCIICase(ariaSort, "none"))
     return SortDirectionNone;
  if (equalIgnoringASCIICase(ariaSort, "ascending"))
     return SortDirectionAscending;
  if (equalIgnoringASCIICase(ariaSort, "descending"))
     return SortDirectionDescending;
  if (equalIgnoringASCIICase(ariaSort, "other"))
     return SortDirectionOther;
   return SortDirectionUndefined;
 }
