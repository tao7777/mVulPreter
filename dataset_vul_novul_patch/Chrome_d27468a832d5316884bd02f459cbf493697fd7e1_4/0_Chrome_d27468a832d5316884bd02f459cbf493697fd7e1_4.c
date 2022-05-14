 bool AXLayoutObject::supportsARIADragging() const {
   const AtomicString& grabbed = getAttribute(aria_grabbedAttr);
  return equalIgnoringASCIICase(grabbed, "true") ||
         equalIgnoringASCIICase(grabbed, "false");
 }
