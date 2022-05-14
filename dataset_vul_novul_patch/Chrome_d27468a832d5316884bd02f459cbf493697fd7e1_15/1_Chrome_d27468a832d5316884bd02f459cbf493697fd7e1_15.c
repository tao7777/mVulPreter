 bool AXNodeObject::isMultiSelectable() const {
   const AtomicString& ariaMultiSelectable =
       getAttribute(aria_multiselectableAttr);
  if (equalIgnoringCase(ariaMultiSelectable, "true"))
     return true;
  if (equalIgnoringCase(ariaMultiSelectable, "false"))
     return false;
 
   return isHTMLSelectElement(getNode()) &&
         toHTMLSelectElement(*getNode()).isMultiple();
}
