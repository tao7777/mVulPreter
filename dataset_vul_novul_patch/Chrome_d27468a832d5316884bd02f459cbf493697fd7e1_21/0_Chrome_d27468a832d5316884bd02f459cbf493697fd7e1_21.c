 const AXObject* AXObject::ariaHiddenRoot() const {
   for (const AXObject* object = this; object; object = object->parentObject()) {
    if (equalIgnoringASCIICase(object->getAttribute(aria_hiddenAttr), "true"))
       return object;
   }
 
  return 0;
}
