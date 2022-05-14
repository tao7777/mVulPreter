 bool WebPageProxy::gestureWillBegin(const IntPoint& point)
 {
    bool canBeginPanning;
     process()->sendSync(Messages::WebPage::GestureWillBegin(point), Messages::WebPage::GestureWillBegin::Reply(canBeginPanning), m_pageID);
     return canBeginPanning;
 }
