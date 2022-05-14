void SimplifyMarkupCommand::doApply()
{
    Node* rootNode = m_firstNode->parentNode();
    Vector<RefPtr<Node> > nodesToRemove;

    for (Node* node = m_firstNode.get(); node && node != m_nodeAfterLast; node = NodeTraversal::next(*node)) {
        if (node->firstChild() || (node->isTextNode() && node->nextSibling()))
            continue;

        Node* startingNode = node->parentNode();
        if (!startingNode)
            continue;
        RenderStyle* startingStyle = startingNode->renderStyle();
        if (!startingStyle)
            continue;
        Node* currentNode = startingNode;
        Node* topNodeWithStartingStyle = 0;
        while (currentNode != rootNode) {
            if (currentNode->parentNode() != rootNode && isRemovableBlock(currentNode))
                nodesToRemove.append(currentNode);

            currentNode = currentNode->parentNode();
            if (!currentNode)
                break;

            if (!currentNode->renderer() || !currentNode->renderer()->isRenderInline() || toRenderInline(currentNode->renderer())->alwaysCreateLineBoxes())
                continue;

            if (currentNode->firstChild() != currentNode->lastChild()) {
                topNodeWithStartingStyle = 0;
                break;
             }
 
             unsigned context;
            if (currentNode->renderStyle()->visualInvalidationDiff(*startingStyle, context).hasNoChange() && !context)
                 topNodeWithStartingStyle = currentNode;
 
         }
        if (topNodeWithStartingStyle) {
            for (Node* node = startingNode; node != topNodeWithStartingStyle; node = node->parentNode())
                nodesToRemove.append(node);
        }
    }

    for (size_t i = 0; i < nodesToRemove.size(); ++i) {
        int numPrunedAncestors = pruneSubsequentAncestorsToRemove(nodesToRemove, i);
        if (numPrunedAncestors < 0)
            continue;
        removeNodePreservingChildren(nodesToRemove[i], AssumeContentIsAlwaysEditable);
        i += numPrunedAncestors;
    }
}
