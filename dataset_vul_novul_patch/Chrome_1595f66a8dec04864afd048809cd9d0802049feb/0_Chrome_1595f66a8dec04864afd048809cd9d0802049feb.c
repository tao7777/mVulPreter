void removeAllDOMObjects()
{
     DOMDataStore& store = DOMData::getCurrentStore();
 
     v8::HandleScope scope;
    ASSERT(!isMainThread());
 
    // Note: We skip the Node wrapper maps because they exist only on the main thread.
     DOMData::removeObjectsFromWrapperMap<void>(&store, store.domObjectMap());
 }
