void PopupContainer::showInRect(const IntRect& r, FrameView* v, int index)
{
    listBox()->setBaseWidth(max(r.width() - kBorderSize * 2, 0));

    listBox()->updateFromElement();


    IntPoint location = v->contentsToWindow(r.location());

     location.move(0, r.height());
 
     m_originalFrameRect = IntRect(location, r.size());
    setFrameRect(m_originalFrameRect);
     showPopup(v);
 }
