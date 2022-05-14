void HTMLDocument::addItemToMap(HashCountedSet<StringImpl*>& map, const AtomicString& name)
 {
     if (name.isEmpty())
         return;
    map.add(name.impl());
     if (Frame* f = frame())
         f->script()->namedItemAdded(this, name);
 }
