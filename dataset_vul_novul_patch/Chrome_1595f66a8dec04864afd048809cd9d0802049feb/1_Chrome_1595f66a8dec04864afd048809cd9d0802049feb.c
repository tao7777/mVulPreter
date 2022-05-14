void removeAllDOMObjects()
{
     DOMDataStore& store = DOMData::getCurrentStore();
 
     v8::HandleScope scope;
 
    if (isMainThread()) {
        DOMData::removeObjectsFromWrapperMap<Node>(&store, store.domNodeMap());
        DOMData::removeObjectsFromWrapperMap<Node>(&store, store.activeDomNodeMap());
    }
     DOMData::removeObjectsFromWrapperMap<void>(&store, store.domObjectMap());
 }
