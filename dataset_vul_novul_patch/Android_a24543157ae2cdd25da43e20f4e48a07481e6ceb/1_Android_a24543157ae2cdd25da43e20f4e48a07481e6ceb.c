 static Maybe<bool> CollectValuesOrEntriesImpl(

       Isolate* isolate, Handle<JSObject> object,
       Handle<FixedArray> values_or_entries, bool get_entries, int* nof_items,
       PropertyFilter filter) {
    int count = 0;
     KeyAccumulator accumulator(isolate, KeyCollectionMode::kOwnOnly,
                                ALL_PROPERTIES);
     Subclass::CollectElementIndicesImpl(
         object, handle(object->elements(), isolate), &accumulator);
     Handle<FixedArray> keys = accumulator.GetKeys();
 
    for (int i = 0; i < keys->length(); ++i) {
       Handle<Object> key(keys->get(i), isolate);
      Handle<Object> value;
       uint32_t index;
       if (!key->ToUint32(&index)) continue;
 
       uint32_t entry = Subclass::GetEntryForIndexImpl(
           isolate, *object, object->elements(), index, filter);
       if (entry == kMaxUInt32) continue;
 
       PropertyDetails details = Subclass::GetDetailsImpl(*object, entry);
 
       if (details.kind() == kData) {
         value = Subclass::GetImpl(isolate, object->elements(), entry);
       } else {
         LookupIterator it(isolate, object, index, LookupIterator::OWN);
         ASSIGN_RETURN_ON_EXCEPTION_VALUE(
             isolate, value, Object::GetProperty(&it), Nothing<bool>());
       }
      if (get_entries) {
        value = MakeEntryPair(isolate, index, value);
       }
       values_or_entries->set(count++, *value);
     }
 
 *nof_items = count;
 return Just(true);
 }
