 AccessibilityButtonState AXObject::checkboxOrRadioValue() const {
   const AtomicString& checkedAttribute =
       getAOMPropertyOrARIAAttribute(AOMStringProperty::kChecked);
  if (equalIgnoringCase(checkedAttribute, "true"))
     return ButtonStateOn;
 
  if (equalIgnoringCase(checkedAttribute, "mixed")) {
     AccessibilityRole role = ariaRoleAttribute();
     if (role == CheckBoxRole || role == MenuItemCheckBoxRole)
      return ButtonStateMixed;
  }

  return ButtonStateOff;
}
