 static Maybe<bool> CollectValuesOrEntriesImpl(

       Isolate* isolate, Handle<JSObject> object,
       Handle<FixedArray> values_or_entries, bool get_entries, int* nof_items,
       PropertyFilter filter) {
    DCHECK_EQ(*nof_items, 0);
     KeyAccumulator accumulator(isolate, KeyCollectionMode::kOwnOnly,
                                ALL_PROPERTIES);
     Subclass::CollectElementIndicesImpl(
         object, handle(object->elements(), isolate), &accumulator);
     Handle<FixedArray> keys = accumulator.GetKeys();
 
    int count = 0;
    int i = 0;
    Handle<Map> original_map(object->map(), isolate);

    for (; i < keys->length(); ++i) {
       Handle<Object> key(keys->get(i), isolate);
       uint32_t index;
       if (!key->ToUint32(&index)) continue;
 
      DCHECK_EQ(object->map(), *original_map);
       uint32_t entry = Subclass::GetEntryForIndexImpl(
           isolate, *object, object->elements(), index, filter);
       if (entry == kMaxUInt32) continue;
 
       PropertyDetails details = Subclass::GetDetailsImpl(*object, entry);
 
      Handle<Object> value;
       if (details.kind() == kData) {
         value = Subclass::GetImpl(isolate, object->elements(), entry);
       } else {
        // This might modify the elements and/or change the elements kind.
         LookupIterator it(isolate, object, index, LookupIterator::OWN);
         ASSIGN_RETURN_ON_EXCEPTION_VALUE(
             isolate, value, Object::GetProperty(&it), Nothing<bool>());
       }
      if (get_entries) value = MakeEntryPair(isolate, index, value);
      values_or_entries->set(count++, *value);
      if (object->map() != *original_map) break;
    }

    // Slow path caused by changes in elements kind during iteration.
    for (; i < keys->length(); i++) {
      Handle<Object> key(keys->get(i), isolate);
      uint32_t index;
      if (!key->ToUint32(&index)) continue;

      if (filter & ONLY_ENUMERABLE) {
        InternalElementsAccessor* accessor =
            reinterpret_cast<InternalElementsAccessor*>(
                object->GetElementsAccessor());
        uint32_t entry = accessor->GetEntryForIndex(isolate, *object,
                                                    object->elements(), index);
        if (entry == kMaxUInt32) continue;
        PropertyDetails details = accessor->GetDetails(*object, entry);
        if (!details.IsEnumerable()) continue;
       }

      Handle<Object> value;
      LookupIterator it(isolate, object, index, LookupIterator::OWN);
      ASSIGN_RETURN_ON_EXCEPTION_VALUE(isolate, value, Object::GetProperty(&it),
                                       Nothing<bool>());

      if (get_entries) value = MakeEntryPair(isolate, index, value);
       values_or_entries->set(count++, *value);
     }
 
 *nof_items = count;
 return Just(true);
 }
