void fillWidgetStates(AXObject& axObject,
                      protocol::Array<AXProperty>& properties) {
  AccessibilityRole role = axObject.roleValue();
  if (roleAllowsChecked(role)) {
    AccessibilityButtonState checked = axObject.checkboxOrRadioValue();
    switch (checked) {
      case ButtonStateOff:
        properties.addItem(
            createProperty(AXWidgetStatesEnum::Checked,
                           createValue("false", AXValueTypeEnum::Tristate)));
        break;
      case ButtonStateOn:
        properties.addItem(
            createProperty(AXWidgetStatesEnum::Checked,
                           createValue("true", AXValueTypeEnum::Tristate)));
        break;
      case ButtonStateMixed:
        properties.addItem(
            createProperty(AXWidgetStatesEnum::Checked,
                           createValue("mixed", AXValueTypeEnum::Tristate)));
        break;
    }
  }

  AccessibilityExpanded expanded = axObject.isExpanded();
  switch (expanded) {
    case ExpandedUndefined:
      break;
    case ExpandedCollapsed:
      properties.addItem(createProperty(
          AXWidgetStatesEnum::Expanded,
          createBooleanValue(false, AXValueTypeEnum::BooleanOrUndefined)));
      break;
    case ExpandedExpanded:
      properties.addItem(createProperty(
          AXWidgetStatesEnum::Expanded,
          createBooleanValue(true, AXValueTypeEnum::BooleanOrUndefined)));
      break;
  }

  if (role == ToggleButtonRole) {
    if (!axObject.isPressed()) {
      properties.addItem(
          createProperty(AXWidgetStatesEnum::Pressed,
                         createValue("false", AXValueTypeEnum::Tristate)));
     } else {
       const AtomicString& pressedAttr =
           axObject.getAttribute(HTMLNames::aria_pressedAttr);
      if (equalIgnoringCase(pressedAttr, "mixed"))
         properties.addItem(
             createProperty(AXWidgetStatesEnum::Pressed,
                            createValue("mixed", AXValueTypeEnum::Tristate)));
      else
        properties.addItem(
            createProperty(AXWidgetStatesEnum::Pressed,
                           createValue("true", AXValueTypeEnum::Tristate)));
    }
  }

  if (roleAllowsSelected(role)) {
    properties.addItem(
        createProperty(AXWidgetStatesEnum::Selected,
                       createBooleanValue(axObject.isSelected())));
  }

  if (roleAllowsModal(role)) {
    properties.addItem(createProperty(AXWidgetStatesEnum::Modal,
                                      createBooleanValue(axObject.isModal())));
  }
}
