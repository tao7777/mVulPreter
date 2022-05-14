IntRect PopupContainer::layoutAndCalculateWidgetRect(int targetControlHeight, const IntPoint& popupInitialCoordinate)
{
    m_listBox->setMaxHeight(kMaxHeight);
    m_listBox->setMaxWidth(std::numeric_limits<int>::max());

    int rtlOffset = layoutAndGetRTLOffset();
    bool isRTL = this->isRTL();
    int rightOffset = isRTL ? rtlOffset : 0;

     IntSize targetSize(m_listBox->width() + kBorderSize * 2,
                        m_listBox->height() + kBorderSize * 2);
 
    IntRect widgetRect;
     ChromeClientChromium* chromeClient = chromeClientChromium();
     if (chromeClient) {
         FloatRect screen = screenAvailableRect(m_frameView.get());
        widgetRect = chromeClient->rootViewToScreen(IntRect(popupInitialCoordinate.x() + rightOffset, popupInitialCoordinate.y(), targetSize.width(), targetSize.height()));
 
         FloatRect windowRect = chromeClient->windowRect();
        if (windowRect.x() >= screen.x() && windowRect.maxX() <= screen.maxX() && (widgetRect.x() < screen.x() || widgetRect.maxX() > screen.maxX())) {
            IntRect inverseWidgetRect = chromeClient->rootViewToScreen(IntRect(popupInitialCoordinate.x() + (isRTL ? 0 : rtlOffset), popupInitialCoordinate.y(), targetSize.width(), targetSize.height()));
             IntRect enclosingScreen = enclosingIntRect(screen);
            unsigned originalCutoff = max(enclosingScreen.x() - widgetRect.x(), 0) + max(widgetRect.maxX() - enclosingScreen.maxX(), 0);
            unsigned inverseCutoff = max(enclosingScreen.x() - inverseWidgetRect.x(), 0) + max(inverseWidgetRect.maxX() - enclosingScreen.maxX(), 0);
 
             if (inverseCutoff < originalCutoff)
                widgetRect = inverseWidgetRect;
            if (widgetRect.x() < screen.x()) {
                unsigned widgetRight = widgetRect.maxX();
                widgetRect.setWidth(widgetRect.maxX() - screen.x());
                widgetRect.setX(widgetRight - widgetRect.width());
                listBox()->setMaxWidthAndLayout(max(widgetRect.width() - kBorderSize * 2, 0));
            } else if (widgetRect.maxX() > screen.maxX()) {
                widgetRect.setWidth(screen.maxX() - widgetRect.x());
                listBox()->setMaxWidthAndLayout(max(widgetRect.width() - kBorderSize * 2, 0));
             }
         }
 
        if (widgetRect.maxY() > static_cast<int>(screen.maxY())) {
            if (widgetRect.y() - widgetRect.height() - targetControlHeight > 0) {
                widgetRect.move(0, -(widgetRect.height() + targetControlHeight));
             } else {
                int spaceAbove = widgetRect.y() - targetControlHeight;
                int spaceBelow = screen.maxY() - widgetRect.y();
                 if (spaceAbove > spaceBelow)
                     m_listBox->setMaxHeight(spaceAbove);
                 else
                    m_listBox->setMaxHeight(spaceBelow);
                layoutAndGetRTLOffset();
                 IntRect frameInScreen = chromeClient->rootViewToScreen(frameRect());
                widgetRect.setY(frameInScreen.y());
                widgetRect.setHeight(frameInScreen.height());
                 if (spaceAbove > spaceBelow)
                    widgetRect.move(0, -(widgetRect.height() + targetControlHeight));
             }
         }
     }
    return widgetRect;
 }
