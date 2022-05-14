void DateTimeChooserImpl::writeDocument(SharedBuffer* data)
{
    String stepString = String::number(m_parameters.step);
    String stepBaseString = String::number(m_parameters.stepBase, 11, WTF::TruncateTrailingZeros);
    IntRect anchorRectInScreen = m_chromeClient->rootViewToScreen(m_parameters.anchorRectInRootView);
    String todayLabelString;
    String otherDateLabelString;
    if (m_parameters.type == InputTypeNames::month) {
        todayLabelString = locale().queryString(WebLocalizedString::ThisMonthButtonLabel);
        otherDateLabelString = locale().queryString(WebLocalizedString::OtherMonthLabel);
    } else if (m_parameters.type == InputTypeNames::week) {
        todayLabelString = locale().queryString(WebLocalizedString::ThisWeekButtonLabel);
        otherDateLabelString = locale().queryString(WebLocalizedString::OtherWeekLabel);
    } else {
        todayLabelString = locale().queryString(WebLocalizedString::CalendarToday);
        otherDateLabelString = locale().queryString(WebLocalizedString::OtherDateLabel);
    }

    addString("<!DOCTYPE html><head><meta charset='UTF-8'><style>\n", data);
    data->append(Platform::current()->loadResource("pickerCommon.css"));
    data->append(Platform::current()->loadResource("pickerButton.css"));
    data->append(Platform::current()->loadResource("suggestionPicker.css"));
    data->append(Platform::current()->loadResource("calendarPicker.css"));
    addString("</style></head><body><div id=main>Loading...</div><script>\n"
        "window.dialogArguments = {\n", data);
    addProperty("anchorRectInScreen", anchorRectInScreen, data);
    addProperty("min", valueToDateTimeString(m_parameters.minimum, m_parameters.type), data);
    addProperty("max", valueToDateTimeString(m_parameters.maximum, m_parameters.type), data);
    addProperty("step", stepString, data);
    addProperty("stepBase", stepBaseString, data);
    addProperty("required", m_parameters.required, data);
    addProperty("currentValue", valueToDateTimeString(m_parameters.doubleValue, m_parameters.type), data);
    addProperty("locale", m_parameters.locale.string(), data);
     addProperty("todayLabel", todayLabelString, data);
     addProperty("clearLabel", locale().queryString(WebLocalizedString::CalendarClear), data);
     addProperty("weekLabel", locale().queryString(WebLocalizedString::WeekNumberLabel), data);
    addProperty("axShowMonthSelector", locale().queryString(WebLocalizedString::AXCalendarShowMonthSelector), data);
    addProperty("axShowNextMonth", locale().queryString(WebLocalizedString::AXCalendarShowNextMonth), data);
    addProperty("axShowPreviousMonth", locale().queryString(WebLocalizedString::AXCalendarShowPreviousMonth), data);
     addProperty("weekStartDay", m_locale->firstDayOfWeek(), data);
     addProperty("shortMonthLabels", m_locale->shortMonthLabels(), data);
     addProperty("dayLabels", m_locale->weekDayShortLabels(), data);
    addProperty("isLocaleRTL", m_locale->isRTL(), data);
    addProperty("isRTL", m_parameters.isAnchorElementRTL, data);
    addProperty("mode", m_parameters.type.string(), data);
    if (m_parameters.suggestions.size()) {
        Vector<String> suggestionValues;
        Vector<String> localizedSuggestionValues;
        Vector<String> suggestionLabels;
        for (unsigned i = 0; i < m_parameters.suggestions.size(); i++) {
            suggestionValues.append(valueToDateTimeString(m_parameters.suggestions[i].value, m_parameters.type));
            localizedSuggestionValues.append(m_parameters.suggestions[i].localizedValue);
            suggestionLabels.append(m_parameters.suggestions[i].label);
        }
        addProperty("suggestionValues", suggestionValues, data);
        addProperty("localizedSuggestionValues", localizedSuggestionValues, data);
        addProperty("suggestionLabels", suggestionLabels, data);
        addProperty("inputWidth", static_cast<unsigned>(m_parameters.anchorRectInRootView.width()), data);
        addProperty("showOtherDateEntry", RenderTheme::theme().supportsCalendarPicker(m_parameters.type), data);
        addProperty("otherDateLabel", otherDateLabelString, data);
        addProperty("suggestionHighlightColor", RenderTheme::theme().activeListBoxSelectionBackgroundColor().serialized(), data);
        addProperty("suggestionHighlightTextColor", RenderTheme::theme().activeListBoxSelectionForegroundColor().serialized(), data);
    }
    addString("}\n", data);

    data->append(Platform::current()->loadResource("pickerCommon.js"));
    data->append(Platform::current()->loadResource("suggestionPicker.js"));
    data->append(Platform::current()->loadResource("calendarPicker.js"));
    addString("</script></body>\n", data);
}
