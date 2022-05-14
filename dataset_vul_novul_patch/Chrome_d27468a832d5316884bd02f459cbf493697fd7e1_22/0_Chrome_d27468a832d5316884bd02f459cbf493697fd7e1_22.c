 AccessibilityButtonState AXObject::checkboxOrRadioValue() const {
   const AtomicString& checkedAttribute =
       getAOMPropertyOrARIAAttribute(AOMStringProperty::kChecked);
  if (equalIgnoringASCIICase(checkedAttribute, "true"))
     return ButtonStateOn;
 
  if (equalIgnoringASCIICase(checkedAttribute, "mixed")) {
     AccessibilityRole role = ariaRoleAttribute();
     if (role == CheckBoxRole || role == MenuItemCheckBoxRole)
      return ButtonStateMixed;
  }

  return ButtonStateOff;
}
