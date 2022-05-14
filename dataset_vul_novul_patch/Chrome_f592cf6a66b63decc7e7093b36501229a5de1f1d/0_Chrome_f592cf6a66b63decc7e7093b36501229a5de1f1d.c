void SVGDocumentExtensions::startAnimations()
{
     WillBeHeapVector<RefPtrWillBeMember<SVGSVGElement> > timeContainers;
     timeContainers.appendRange(m_timeContainers.begin(), m_timeContainers.end());
     WillBeHeapVector<RefPtrWillBeMember<SVGSVGElement> >::iterator end = timeContainers.end();
    for (WillBeHeapVector<RefPtrWillBeMember<SVGSVGElement> >::iterator itr = timeContainers.begin(); itr != end; ++itr) {
        SMILTimeContainer* timeContainer = (*itr)->timeContainer();
        if (!timeContainer->isStarted())
            timeContainer->begin();
    }
 }
