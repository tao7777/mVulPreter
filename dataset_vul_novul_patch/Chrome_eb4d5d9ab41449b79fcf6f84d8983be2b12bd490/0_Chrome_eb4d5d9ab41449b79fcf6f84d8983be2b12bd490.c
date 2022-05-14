void ContainerNode::notifyNodeInsertedInternal(Node& root, NodeVector& postInsertionNotificationTargets)
{
    EventDispatchForbiddenScope assertNoEventDispatch;
    ScriptForbiddenScope forbidScript;
 
     for (Node& node : NodeTraversal::inclusiveDescendantsOf(root)) {
        // into detached subtrees that are not in a shadow tree.
        if (!inDocument() && !isInShadowTree() && !node.isContainerNode())
             continue;
         if (Node::InsertionShouldCallDidNotifySubtreeInsertions == node.insertedInto(this))
             postInsertionNotificationTargets.append(&node);
        for (ShadowRoot* shadowRoot = node.youngestShadowRoot(); shadowRoot; shadowRoot = shadowRoot->olderShadowRoot())
            notifyNodeInsertedInternal(*shadowRoot, postInsertionNotificationTargets);
    }
}
