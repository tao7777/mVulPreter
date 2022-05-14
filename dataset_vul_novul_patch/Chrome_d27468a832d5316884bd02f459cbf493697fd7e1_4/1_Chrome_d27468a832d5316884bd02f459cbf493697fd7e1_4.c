 bool AXLayoutObject::supportsARIADragging() const {
   const AtomicString& grabbed = getAttribute(aria_grabbedAttr);
  return equalIgnoringCase(grabbed, "true") ||
         equalIgnoringCase(grabbed, "false");
 }
