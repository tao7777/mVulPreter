 JSRetainPtr<JSStringRef> AccessibilityUIElement::stringValue()
 {
    if (!m_element || !ATK_IS_TEXT(m_element))
        return JSStringCreateWithCharacters(0, 0);

    GOwnPtr<gchar> text(atk_text_get_text(ATK_TEXT(m_element), 0, -1));
    GOwnPtr<gchar> textWithReplacedCharacters(replaceCharactersForResults(text.get()));
    GOwnPtr<gchar> axValue(g_strdup_printf("AXValue: %s", textWithReplacedCharacters.get()));

    return JSStringCreateWithUTF8CString(axValue.get());
 }
