RGBA32 AXNodeObject::colorValue() const {
  if (!isHTMLInputElement(getNode()) || !isColorWell())
    return AXObject::colorValue();
 
   HTMLInputElement* input = toHTMLInputElement(getNode());
   const AtomicString& type = input->getAttribute(typeAttr);
  if (!equalIgnoringASCIICase(type, "color"))
     return AXObject::colorValue();
 
  Color color;
  bool success = color.setFromString(input->value());
  DCHECK(success);
  return color.rgb();
}
