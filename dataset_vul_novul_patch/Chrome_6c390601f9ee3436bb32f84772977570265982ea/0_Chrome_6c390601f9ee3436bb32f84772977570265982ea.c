bool ContainerNode::replaceChild(PassRefPtr<Node> newChild, Node* oldChild, ExceptionCode& ec, bool shouldLazyAttach)
{
    ASSERT(refCount() || parentOrHostNode());

    RefPtr<Node> protect(this);

    ec = 0;

    if (oldChild == newChild) // nothing to do
        return true;

    checkReplaceChild(newChild.get(), oldChild, ec);
    if (ec)
        return false;

    if (!oldChild || oldChild->parentNode() != this) {
        ec = NOT_FOUND_ERR;
        return false;
    }

#if ENABLE(MUTATION_OBSERVERS)
    ChildListMutationScope mutation(this);
#endif

    RefPtr<Node> next = oldChild->nextSibling();

    RefPtr<Node> removedChild = oldChild;
    removeChild(oldChild, ec);
    if (ec)
        return false;

    if (next && (next->previousSibling() == newChild || next == newChild)) // nothing to do
        return true;

    checkReplaceChild(newChild.get(), oldChild, ec);
    if (ec)
        return false;

    NodeVector targets;
    collectChildrenAndRemoveFromOldParent(newChild.get(), targets, ec);
     if (ec)
         return false;
 
    // Does this yet another check because collectChildrenAndRemoveFromOldParent() fires a MutationEvent.
    checkReplaceChild(newChild.get(), oldChild, ec);
    if (ec)
        return false;

     InspectorInstrumentation::willInsertDOMNode(document(), this);
 
    for (NodeVector::const_iterator it = targets.begin(); it != targets.end(); ++it) {
        Node* child = it->get();

        if (next && next->parentNode() != this)
            break;
        if (child->parentNode())
            break;

        treeScope()->adoptIfNeeded(child);

        forbidEventDispatch();
        if (next)
            insertBeforeCommon(next.get(), child);
        else
            appendChildToContainer(child, this);
        allowEventDispatch();

        updateTreeAfterInsertion(this, child, shouldLazyAttach);
    }

    dispatchSubtreeModifiedEvent();
    return true;
}
