bool AXLayoutObject::isSelected() const {
  if (!getLayoutObject() || !getNode())
     return false;
 
   const AtomicString& ariaSelected = getAttribute(aria_selectedAttr);
  if (equalIgnoringASCIICase(ariaSelected, "true"))
     return true;
 
   AXObject* focusedObject = axObjectCache().focusedObject();
  if (ariaRoleAttribute() == ListBoxOptionRole && focusedObject &&
      focusedObject->activeDescendant() == this) {
    return true;
  }

  if (isTabItem() && isTabItemSelected())
    return true;

  return false;
}
