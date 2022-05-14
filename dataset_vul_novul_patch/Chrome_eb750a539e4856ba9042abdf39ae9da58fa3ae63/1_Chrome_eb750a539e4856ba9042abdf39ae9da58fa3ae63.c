NodeIterator::NodeIterator(PassRefPtrWillBeRawPtr<Node> rootNode, unsigned whatToShow, PassRefPtrWillBeRawPtr<NodeFilter> filter)
     : NodeIteratorBase(rootNode, whatToShow, filter)
     , m_referenceNode(root(), true)
 {
    root()->document().attachNodeIterator(this);
 }
