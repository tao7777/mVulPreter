 void PopupContainer::showPopup(FrameView* view)
{
    m_frameView = view;
    listBox()->m_focusedNode = m_frameView->frame()->document()->focusedNode();
 
     ChromeClientChromium* chromeClient = chromeClientChromium();
     if (chromeClient) {
        IntRect popupRect = m_originalFrameRect;
         chromeClient->popupOpened(this, layoutAndCalculateWidgetRect(popupRect.height(), popupRect.location()), false);
         m_popupOpen = true;
     }

    if (!m_listBox->parent())
        addChild(m_listBox.get());

    m_listBox->setVerticalScrollbarMode(ScrollbarAuto);

    m_listBox->scrollToRevealSelection();

    invalidate();
}
