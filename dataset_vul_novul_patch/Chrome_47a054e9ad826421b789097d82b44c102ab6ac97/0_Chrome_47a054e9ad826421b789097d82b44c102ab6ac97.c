PageGroupLoadDeferrer::PageGroupLoadDeferrer(Page* page, bool deferSelf)
{
    const HashSet<Page*>& pages = page->group().pages();

    HashSet<Page*>::const_iterator end = pages.end();
    for (HashSet<Page*>::const_iterator it = pages.begin(); it != end; ++it) {
        Page* otherPage = *it;
        if ((deferSelf || otherPage != page)) {
             if (!otherPage->defersLoading()) {
                 m_deferredFrames.append(otherPage->mainFrame());
 
                // Ensure that we notify the client if the initial empty document is accessed before showing anything
                // modal, to prevent spoofs while the modal window or sheet is visible.
                otherPage->mainFrame()->loader()->notifyIfInitialDocumentAccessed();

                 for (Frame* frame = otherPage->mainFrame(); frame; frame = frame->tree()->traverseNext())
                    frame->document()->suspendScheduledTasks(ActiveDOMObject::WillDeferLoading);
            }
        }
    }

    size_t count = m_deferredFrames.size();
    for (size_t i = 0; i < count; ++i)
        if (Page* page = m_deferredFrames[i]->page())
            page->setDefersLoading(true);
}
