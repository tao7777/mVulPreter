bool AXNodeObject::isPressed() const {
  if (!isButton())
    return false;

  Node* node = this->getNode();
  if (!node)
    return false;

   if (ariaRoleAttribute() == ToggleButtonRole) {
    if (equalIgnoringASCIICase(getAttribute(aria_pressedAttr), "true") ||
        equalIgnoringASCIICase(getAttribute(aria_pressedAttr), "mixed"))
       return true;
     return false;
   }

  return node->isActive();
}
