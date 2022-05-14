 bool SVGElement::HasSVGParent() const {
  Element* parent = FlatTreeTraversal::ParentElement(*this);
  return parent && parent->IsSVGElement();
 }
