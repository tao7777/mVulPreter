void HTMLDocument::removeItemFromMap(HashCountedSet<StringImpl*>& map, const AtomicString& name)
 {
     if (name.isEmpty())
         return;
    map.remove(name.impl());
     if (Frame* f = frame())
         f->script()->namedItemRemoved(this, name);
 }
