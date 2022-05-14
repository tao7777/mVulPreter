static void willRemoveChildren(ContainerNode* container)
{
     NodeVector children;
     getChildNodes(container, children);
 
    container->document().nodeChildrenWillBeRemoved(container);
     ChildListMutationScope mutation(container);
     for (NodeVector::const_iterator it = children.begin(); it != children.end(); it++) {
         Node* child = it->get();
        mutation.willRemoveChild(child);
        child->notifyMutationObserversNodeWillDetach();

        dispatchChildRemovalEvents(child);
    }

    ChildFrameDisconnector(container).disconnect(ChildFrameDisconnector::DescendantsOnly);
}
