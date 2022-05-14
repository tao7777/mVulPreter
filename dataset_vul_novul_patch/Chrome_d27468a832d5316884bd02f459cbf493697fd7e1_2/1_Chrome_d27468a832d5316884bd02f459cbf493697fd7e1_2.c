bool AXLayoutObject::elementAttributeValue(
    const QualifiedName& attributeName) const {
   if (!m_layoutObject)
     return false;
 
  return equalIgnoringCase(getAttribute(attributeName), "true");
 }
