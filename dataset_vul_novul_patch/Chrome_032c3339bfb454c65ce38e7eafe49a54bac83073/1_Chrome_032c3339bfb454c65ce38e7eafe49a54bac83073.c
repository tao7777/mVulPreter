 bool SVGElement::HasSVGParent() const {
  return ParentOrShadowHostElement() &&
         ParentOrShadowHostElement()->IsSVGElement();
 }
