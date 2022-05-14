 void PopupContainer::refresh(const IntRect& targetControlRect)
 {
    listBox()->setBaseWidth(max(m_originalFrameRect.width() - kBorderSize * 2, 0));
    listBox()->updateFromElement();

    IntPoint locationInWindow = m_frameView->contentsToWindow(targetControlRect.location());

    locationInWindow.move(0, targetControlRect.height());
 
    IntRect widgetRectInScreen = layoutAndCalculateWidgetRect(targetControlRect.height(), locationInWindow);
 
    // Reset the size (which can be set to the PopupListBox size in layoutAndGetRTLOffset(), exceeding the available widget rectangle.)
    if (size() != widgetRectInScreen.size())
        resize(widgetRectInScreen.size());

    ChromeClientChromium* chromeClient = chromeClientChromium();
    if (chromeClient) {
        // Update the WebWidget location (which is relative to the screen origin).
        if (widgetRectInScreen != chromeClient->windowRect())
            chromeClient->setWindowRect(widgetRectInScreen);
     }
 
     invalidate();
}
