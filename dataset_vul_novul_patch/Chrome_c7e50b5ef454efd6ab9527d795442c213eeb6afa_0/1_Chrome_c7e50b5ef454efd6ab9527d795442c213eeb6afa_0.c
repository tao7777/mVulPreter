 void PopupContainer::refresh(const IntRect& targetControlRect)
 {
    IntPoint location = m_frameView->contentsToWindow(targetControlRect.location());
    location.move(0, targetControlRect.height());
 
    listBox()->setBaseWidth(max(m_originalFrameRect.width() - kBorderSize * 2, 0));
 
    listBox()->updateFromElement();
    IntSize originalSize = size();
    IntRect widgetRect = layoutAndCalculateWidgetRect(targetControlRect.height(), location);
    if (originalSize != widgetRect.size()) {
        ChromeClientChromium* chromeClient = chromeClientChromium();
        if (chromeClient) {
            IntPoint widgetLocation = chromeClient->screenToRootView(widgetRect.location());
            widgetRect.setLocation(widgetLocation);
            setFrameRect(widgetRect);
        }
     }
 
     invalidate();
}
