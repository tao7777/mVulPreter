AccessibilityOrientation AXNodeObject::orientation() const {
   const AtomicString& ariaOrientation =
       getAOMPropertyOrARIAAttribute(AOMStringProperty::kOrientation);
   AccessibilityOrientation orientation = AccessibilityOrientationUndefined;
  if (equalIgnoringASCIICase(ariaOrientation, "horizontal"))
     orientation = AccessibilityOrientationHorizontal;
  else if (equalIgnoringASCIICase(ariaOrientation, "vertical"))
     orientation = AccessibilityOrientationVertical;
 
   switch (roleValue()) {
    case ComboBoxRole:
    case ListBoxRole:
    case MenuRole:
    case ScrollBarRole:
    case TreeRole:
      if (orientation == AccessibilityOrientationUndefined)
        orientation = AccessibilityOrientationVertical;

      return orientation;
    case MenuBarRole:
    case SliderRole:
    case SplitterRole:
    case TabListRole:
    case ToolbarRole:
      if (orientation == AccessibilityOrientationUndefined)
        orientation = AccessibilityOrientationHorizontal;

      return orientation;
    case RadioGroupRole:
    case TreeGridRole:
    case TableRole:
      return orientation;
    default:
      return AXObject::orientation();
  }
}
