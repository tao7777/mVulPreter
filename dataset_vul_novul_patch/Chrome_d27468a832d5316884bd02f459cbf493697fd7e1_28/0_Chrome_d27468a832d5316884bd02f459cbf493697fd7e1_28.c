bool nodeHasRole(Node* node, const String& role) {
   if (!node || !node->isElementNode())
     return false;
 
  return equalIgnoringASCIICase(toElement(node)->getAttribute(roleAttr), role);
 }
