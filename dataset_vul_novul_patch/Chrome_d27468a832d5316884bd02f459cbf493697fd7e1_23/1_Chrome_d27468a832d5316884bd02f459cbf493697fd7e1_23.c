 const AXObject* AXObject::disabledAncestor() const {
   const AtomicString& disabled = getAttribute(aria_disabledAttr);
  if (equalIgnoringCase(disabled, "true"))
     return this;
  if (equalIgnoringCase(disabled, "false"))
     return 0;
 
   if (AXObject* parent = parentObject())
    return parent->disabledAncestor();

  return 0;
}
