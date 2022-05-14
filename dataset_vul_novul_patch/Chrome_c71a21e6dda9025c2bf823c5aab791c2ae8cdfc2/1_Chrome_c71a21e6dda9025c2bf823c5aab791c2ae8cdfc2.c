void ContainerNode::parserInsertBefore(PassRefPtrWillBeRawPtr<Node> newChild, Node& nextChild)
{
    ASSERT(newChild);
    ASSERT(nextChild.parentNode() == this);
    ASSERT(!newChild->isDocumentFragment());
    ASSERT(!isHTMLTemplateElement(this));

    if (nextChild.previousSibling() == newChild || &nextChild == newChild) // nothing to do
        return;

    if (!checkParserAcceptChild(*newChild))
        return;

    RefPtrWillBeRawPtr<Node> protect(this);

     while (RefPtrWillBeRawPtr<ContainerNode> parent = newChild->parentNode())
         parent->parserRemoveChild(*newChild);
 
     if (document() != newChild->document())
         document().adoptNode(newChild.get(), ASSERT_NO_EXCEPTION);
 
    {
        EventDispatchForbiddenScope assertNoEventDispatch;
        ScriptForbiddenScope forbidScript;

        treeScope().adoptIfNeeded(*newChild);
        insertBeforeCommon(nextChild, *newChild);
        newChild->updateAncestorConnectedSubframeCountForInsertion();
        ChildListMutationScope(*this).childAdded(*newChild);
    }

    notifyNodeInserted(*newChild, ChildrenChangeSourceParser);
}
