void BaseMultipleFieldsDateAndTimeInputType::didFocusOnControl()
{
 
    if (!containsFocusedShadowElement())
        return;
     element()->setFocus(true);
}
