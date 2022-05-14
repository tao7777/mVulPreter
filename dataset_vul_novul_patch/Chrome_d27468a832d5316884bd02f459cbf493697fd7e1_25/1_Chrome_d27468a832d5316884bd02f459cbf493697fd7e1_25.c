 bool AXObject::isLiveRegion() const {
   const AtomicString& liveRegion = liveRegionStatus();
  return equalIgnoringCase(liveRegion, "polite") ||
         equalIgnoringCase(liveRegion, "assertive");
 }
