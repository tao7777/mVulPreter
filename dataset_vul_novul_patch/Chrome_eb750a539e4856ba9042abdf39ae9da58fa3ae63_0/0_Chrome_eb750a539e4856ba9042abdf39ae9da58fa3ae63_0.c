 NodeIterator::~NodeIterator()
 {
    if (!root()->isAttributeNode())
        root()->document().detachNodeIterator(this);
 }
