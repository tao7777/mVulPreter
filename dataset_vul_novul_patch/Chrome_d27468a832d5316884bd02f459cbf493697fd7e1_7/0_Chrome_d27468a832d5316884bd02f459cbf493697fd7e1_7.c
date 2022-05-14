 bool AXNodeObject::canSetValueAttribute() const {
  if (equalIgnoringASCIICase(getAttribute(aria_readonlyAttr), "true"))
     return false;
 
   if (isProgressIndicator() || isSlider())
    return true;

  if (isTextControl() && !isNativeTextControl())
    return true;

  return !isReadOnly();
}
