bool HTMLFormControlElement::isAutofocusable() const
 {
    if (!fastHasAttribute(autofocusAttr))
        return false;
    if (hasTagName(inputTag))
        return !toHTMLInputElement(this)->isInputTypeHidden();
    if (hasTagName(selectTag))
        return true;
    if (hasTagName(keygenTag))
        return true;
    if (hasTagName(buttonTag))
        return true;
    if (hasTagName(textareaTag))
        return true;
     return false;
 }
