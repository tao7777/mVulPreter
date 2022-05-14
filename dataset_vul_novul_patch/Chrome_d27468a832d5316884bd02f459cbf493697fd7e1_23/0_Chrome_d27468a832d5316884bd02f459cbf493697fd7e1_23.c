 const AXObject* AXObject::disabledAncestor() const {
   const AtomicString& disabled = getAttribute(aria_disabledAttr);
  if (equalIgnoringASCIICase(disabled, "true"))
     return this;
  if (equalIgnoringASCIICase(disabled, "false"))
     return 0;
 
   if (AXObject* parent = parentObject())
    return parent->disabledAncestor();

  return 0;
}
