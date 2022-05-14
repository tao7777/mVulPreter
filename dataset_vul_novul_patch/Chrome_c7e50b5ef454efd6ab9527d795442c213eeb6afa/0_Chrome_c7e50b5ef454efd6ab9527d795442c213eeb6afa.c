IntRect PopupContainer::layoutAndCalculateWidgetRect(int targetControlHeight, const IntPoint& popupInitialCoordinate)
{
    m_listBox->setMaxHeight(kMaxHeight);
    m_listBox->setMaxWidth(std::numeric_limits<int>::max());

    int rtlOffset = layoutAndGetRTLOffset();
    bool isRTL = this->isRTL();
    int rightOffset = isRTL ? rtlOffset : 0;

     IntSize targetSize(m_listBox->width() + kBorderSize * 2,
                        m_listBox->height() + kBorderSize * 2);
 
    IntRect widgetRectInScreen;
     ChromeClientChromium* chromeClient = chromeClientChromium();
     if (chromeClient) {
         FloatRect screen = screenAvailableRect(m_frameView.get());
        widgetRectInScreen = chromeClient->rootViewToScreen(IntRect(popupInitialCoordinate.x() + rightOffset, popupInitialCoordinate.y(), targetSize.width(), targetSize.height()));
 
         FloatRect windowRect = chromeClient->windowRect();
        if (windowRect.x() >= screen.x() && windowRect.maxX() <= screen.maxX() && (widgetRectInScreen.x() < screen.x() || widgetRectInScreen.maxX() > screen.maxX())) {
            IntRect inverseWidgetRectInScreen = chromeClient->rootViewToScreen(IntRect(popupInitialCoordinate.x() + (isRTL ? 0 : rtlOffset), popupInitialCoordinate.y(), targetSize.width(), targetSize.height()));
             IntRect enclosingScreen = enclosingIntRect(screen);
            unsigned originalCutoff = max(enclosingScreen.x() - widgetRectInScreen.x(), 0) + max(widgetRectInScreen.maxX() - enclosingScreen.maxX(), 0);
            unsigned inverseCutoff = max(enclosingScreen.x() - inverseWidgetRectInScreen.x(), 0) + max(inverseWidgetRectInScreen.maxX() - enclosingScreen.maxX(), 0);
 
             if (inverseCutoff < originalCutoff)
              widgetRectInScreen = inverseWidgetRectInScreen;

            if (widgetRectInScreen.x() < screen.x()) {
                unsigned widgetRight = widgetRectInScreen.maxX();
                widgetRectInScreen.setWidth(widgetRectInScreen.maxX() - screen.x());
                widgetRectInScreen.setX(widgetRight - widgetRectInScreen.width());
                listBox()->setMaxWidthAndLayout(max(widgetRectInScreen.width() - kBorderSize * 2, 0));
            } else if (widgetRectInScreen.maxX() > screen.maxX()) {
                widgetRectInScreen.setWidth(screen.maxX() - widgetRectInScreen.x());
                listBox()->setMaxWidthAndLayout(max(widgetRectInScreen.width() - kBorderSize * 2, 0));
             }
         }
 
        if (widgetRectInScreen.maxY() > static_cast<int>(screen.maxY())) {
            if (widgetRectInScreen.y() - widgetRectInScreen.height() - targetControlHeight > 0) {
                widgetRectInScreen.move(0, -(widgetRectInScreen.height() + targetControlHeight));
             } else {
                int spaceAbove = widgetRectInScreen.y() - targetControlHeight;
                int spaceBelow = screen.maxY() - widgetRectInScreen.y();
                 if (spaceAbove > spaceBelow)
                     m_listBox->setMaxHeight(spaceAbove);
                 else
                    m_listBox->setMaxHeight(spaceBelow);
                layoutAndGetRTLOffset();
                 IntRect frameInScreen = chromeClient->rootViewToScreen(frameRect());
                widgetRectInScreen.setY(frameInScreen.y());
                widgetRectInScreen.setHeight(frameInScreen.height());
                 if (spaceAbove > spaceBelow)
                    widgetRectInScreen.move(0, -(widgetRectInScreen.height() + targetControlHeight));
             }
         }
     }

    return widgetRectInScreen;
 }
