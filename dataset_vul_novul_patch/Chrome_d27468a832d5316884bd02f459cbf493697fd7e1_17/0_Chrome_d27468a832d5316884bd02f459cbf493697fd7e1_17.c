bool AXNodeObject::isRequired() const {
  Node* n = this->getNode();
  if (n && (n->isElementNode() && toElement(n)->isFormControlElement()) &&
       hasAttribute(requiredAttr))
     return toHTMLFormControlElement(n)->isRequired();
 
  if (equalIgnoringASCIICase(getAttribute(aria_requiredAttr), "true"))
     return true;
 
   return false;
}
