 bool AXNodeObject::isMultiSelectable() const {
   const AtomicString& ariaMultiSelectable =
       getAttribute(aria_multiselectableAttr);
  if (equalIgnoringASCIICase(ariaMultiSelectable, "true"))
     return true;
  if (equalIgnoringASCIICase(ariaMultiSelectable, "false"))
     return false;
 
   return isHTMLSelectElement(getNode()) &&
         toHTMLSelectElement(*getNode()).isMultiple();
}
