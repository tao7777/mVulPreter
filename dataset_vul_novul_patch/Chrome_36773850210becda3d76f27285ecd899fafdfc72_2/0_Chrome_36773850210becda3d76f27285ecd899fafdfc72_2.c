void HTMLDocument::addItemToMap(HashCountedSet<StringImpl*>& map, const AtomicString& name)
void HTMLDocument::addItemToMap(HashCountedSet<AtomicString>& map, const AtomicString& name)
 {
     if (name.isEmpty())
         return;
    map.add(name);
     if (Frame* f = frame())
         f->script()->namedItemAdded(this, name);
 }
