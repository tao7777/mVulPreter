AXObjectInclusion AXLayoutObject::defaultObjectInclusion(
    IgnoredReasons* ignoredReasons) const {

  if (!m_layoutObject) {
    if (ignoredReasons)
      ignoredReasons->push_back(IgnoredReason(AXNotRendered));
    return IgnoreObject;
  }

   if (m_layoutObject->style()->visibility() != EVisibility::kVisible) {
    if (equalIgnoringASCIICase(getAttribute(aria_hiddenAttr), "false"))
       return DefaultBehavior;
 
     if (ignoredReasons)
      ignoredReasons->push_back(IgnoredReason(AXNotVisible));
    return IgnoreObject;
  }

  return AXObject::defaultObjectInclusion(ignoredReasons);
}
