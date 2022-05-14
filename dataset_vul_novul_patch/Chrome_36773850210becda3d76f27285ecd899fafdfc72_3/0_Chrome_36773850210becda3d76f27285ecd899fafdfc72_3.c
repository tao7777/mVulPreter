void HTMLDocument::removeItemFromMap(HashCountedSet<StringImpl*>& map, const AtomicString& name)
void HTMLDocument::removeItemFromMap(HashCountedSet<AtomicString>& map, const AtomicString& name)
 {
     if (name.isEmpty())
         return;
    map.remove(name);
     if (Frame* f = frame())
         f->script()->namedItemRemoved(this, name);
 }
