NodeIterator::NodeIterator(PassRefPtrWillBeRawPtr<Node> rootNode, unsigned whatToShow, PassRefPtrWillBeRawPtr<NodeFilter> filter)
     : NodeIteratorBase(rootNode, whatToShow, filter)
     , m_referenceNode(root(), true)
 {
    // If NodeIterator target is Attr node, don't subscribe for nodeWillBeRemoved, as it would never have child nodes.
    if (!root()->isAttributeNode())
        root()->document().attachNodeIterator(this);
 }
