static Element* siblingWithAriaRole(String role, Node* node) {
  Node* parent = node->parentNode();
  if (!parent)
    return 0;

  for (Element* sibling = ElementTraversal::firstChild(*parent); sibling;
        sibling = ElementTraversal::nextSibling(*sibling)) {
     const AtomicString& siblingAriaRole =
         AccessibleNode::getProperty(sibling, AOMStringProperty::kRole);
    if (equalIgnoringASCIICase(siblingAriaRole, role))
       return sibling;
   }
 
  return 0;
}
