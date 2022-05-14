bool AXListBoxOption::isEnabled() const {
   if (!getNode())
     return false;
 
  if (equalIgnoringASCIICase(getAttribute(aria_disabledAttr), "true"))
     return false;
 
   if (toElement(getNode())->hasAttribute(disabledAttr))
    return false;

  return true;
}
