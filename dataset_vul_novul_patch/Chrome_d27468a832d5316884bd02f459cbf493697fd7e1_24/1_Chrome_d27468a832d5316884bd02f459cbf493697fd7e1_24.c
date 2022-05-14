 bool AXObject::isHiddenForTextAlternativeCalculation() const {
  if (equalIgnoringCase(getAttribute(aria_hiddenAttr), "false"))
     return false;
 
   if (getLayoutObject())
    return getLayoutObject()->style()->visibility() != EVisibility::kVisible;

  Document* document = getDocument();
  if (!document || !document->frame())
    return false;
  if (Node* node = getNode()) {
    if (node->isConnected() && node->isElementNode()) {
      RefPtr<ComputedStyle> style =
          document->ensureStyleResolver().styleForElement(toElement(node));
      return style->display() == EDisplay::kNone ||
             style->visibility() != EVisibility::kVisible;
    }
  }
  return false;
}
