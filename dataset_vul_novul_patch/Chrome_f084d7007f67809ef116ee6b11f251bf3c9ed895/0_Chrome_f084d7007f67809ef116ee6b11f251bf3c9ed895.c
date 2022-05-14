void ContainerNode::removeChildren()
{
    if (!m_firstChild)
        return;

    RefPtr<ContainerNode> protect(this);

    if (FullscreenElementStack* fullscreen = FullscreenElementStack::fromIfExists(&document()))
        fullscreen->removeFullScreenElementOfSubtree(this, true);

    willRemoveChildren(protect.get());

    {
        SubframeLoadingDisabler disabler(this);

         document().removeFocusedElementOfSubtree(this, true);
     }
 
    document().nodeChildrenWillBeRemoved(this);

     NodeVector removedChildren;
     {
         RenderWidget::UpdateSuspendScope suspendWidgetHierarchyUpdates;
        {
            NoEventDispatchAssertion assertNoEventDispatch;
            removedChildren.reserveInitialCapacity(childNodeCount());
            while (m_firstChild) {
                removedChildren.append(m_firstChild);
                removeBetween(0, m_firstChild->nextSibling(), m_firstChild);
            }
        }

        childrenChanged(false, 0, 0, -static_cast<int>(removedChildren.size()));

        for (size_t i = 0; i < removedChildren.size(); ++i)
            ChildNodeRemovalNotifier(this).notify(removedChildren[i].get());
    }

    dispatchSubtreeModifiedEvent();
}
