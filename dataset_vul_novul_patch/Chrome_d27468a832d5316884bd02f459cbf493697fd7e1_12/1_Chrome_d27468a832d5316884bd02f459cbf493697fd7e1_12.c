bool AXNodeObject::isChecked() const {
  Node* node = this->getNode();
  if (!node)
    return false;

  if (isHTMLInputElement(*node))
    return toHTMLInputElement(*node).shouldAppearChecked();

  switch (ariaRoleAttribute()) {
    case CheckBoxRole:
    case MenuItemCheckBoxRole:
     case MenuItemRadioRole:
     case RadioButtonRole:
     case SwitchRole:
      if (equalIgnoringCase(
               getAOMPropertyOrARIAAttribute(AOMStringProperty::kChecked),
               "true"))
         return true;
      return false;
    default:
      break;
  }

  return false;
}
