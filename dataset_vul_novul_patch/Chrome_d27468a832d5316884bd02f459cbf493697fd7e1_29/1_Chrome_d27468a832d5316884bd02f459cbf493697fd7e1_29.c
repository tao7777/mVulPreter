SortDirection AXTableCell::getSortDirection() const {
  if (roleValue() != RowHeaderRole && roleValue() != ColumnHeaderRole)
    return SortDirectionUndefined;

  const AtomicString& ariaSort =
       getAOMPropertyOrARIAAttribute(AOMStringProperty::kSort);
   if (ariaSort.isEmpty())
     return SortDirectionUndefined;
  if (equalIgnoringCase(ariaSort, "none"))
     return SortDirectionNone;
  if (equalIgnoringCase(ariaSort, "ascending"))
     return SortDirectionAscending;
  if (equalIgnoringCase(ariaSort, "descending"))
     return SortDirectionDescending;
  if (equalIgnoringCase(ariaSort, "other"))
     return SortDirectionOther;
   return SortDirectionUndefined;
 }
