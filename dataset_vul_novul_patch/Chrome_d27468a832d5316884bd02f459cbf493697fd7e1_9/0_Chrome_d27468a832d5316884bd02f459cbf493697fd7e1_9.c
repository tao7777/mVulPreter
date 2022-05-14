bool AXNodeObject::computeAccessibilityIsIgnored(
    IgnoredReasons* ignoredReasons) const {
#if DCHECK_IS_ON()
  ASSERT(m_initialized);
#endif

  if (isDescendantOfLeafNode()) {
    if (ignoredReasons)
      ignoredReasons->push_back(
          IgnoredReason(AXAncestorIsLeafNode, leafNodeAncestor()));
    return true;
  }

  AXObject* controlObject = correspondingControlForLabelElement();
  if (controlObject && controlObject->isCheckboxOrRadio() &&
      controlObject->nameFromLabelElement()) {
    if (ignoredReasons) {
      HTMLLabelElement* label = labelElementContainer();
      if (label && label != getNode()) {
        AXObject* labelAXObject = axObjectCache().getOrCreate(label);
        ignoredReasons->push_back(
            IgnoredReason(AXLabelContainer, labelAXObject));
      }

      ignoredReasons->push_back(IgnoredReason(AXLabelFor, controlObject));
    }
    return true;
  }

   Element* element = getNode()->isElementNode() ? toElement(getNode())
                                                 : getNode()->parentElement();
   if (!getLayoutObject() && (!element || !element->isInCanvasSubtree()) &&
      !equalIgnoringASCIICase(getAttribute(aria_hiddenAttr), "false")) {
     if (ignoredReasons)
       ignoredReasons->push_back(IgnoredReason(AXNotRendered));
     return true;
  }

  if (m_role == UnknownRole) {
    if (ignoredReasons)
      ignoredReasons->push_back(IgnoredReason(AXUninteresting));
    return true;
  }
  return false;
}
