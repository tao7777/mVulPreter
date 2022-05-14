bool AXObject::isMultiline() const {
  Node* node = this->getNode();
  if (!node)
    return false;

  if (isHTMLTextAreaElement(*node))
    return true;

  if (hasEditableStyle(*node))
    return true;

   if (!isNativeTextControl() && !isNonNativeTextControl())
     return false;
 
  return equalIgnoringCase(getAttribute(aria_multilineAttr), "true");
 }
