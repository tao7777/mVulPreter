bool AXNodeObject::hasContentEditableAttributeSet() const {
  const AtomicString& contentEditableValue = getAttribute(contenteditableAttr);
  if (contentEditableValue.isNull())
     return false;
   return contentEditableValue.isEmpty() ||
         equalIgnoringASCIICase(contentEditableValue, "true");
 }
