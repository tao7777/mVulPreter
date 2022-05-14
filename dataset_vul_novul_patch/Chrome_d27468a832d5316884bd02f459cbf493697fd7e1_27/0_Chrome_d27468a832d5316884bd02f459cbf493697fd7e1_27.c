bool isNodeAriaVisible(Node* node) {
  if (!node)
    return false;

   if (!node->isElementNode())
     return false;
 
  return equalIgnoringASCIICase(toElement(node)->getAttribute(aria_hiddenAttr),
                                "false");
 }
