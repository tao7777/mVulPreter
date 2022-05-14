 bool AXObject::isLiveRegion() const {
   const AtomicString& liveRegion = liveRegionStatus();
  return equalIgnoringASCIICase(liveRegion, "polite") ||
         equalIgnoringASCIICase(liveRegion, "assertive");
 }
