 NodeIterator::~NodeIterator()
 {
    root()->document().detachNodeIterator(this);
 }
