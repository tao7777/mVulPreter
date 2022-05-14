 static Maybe<bool> IncludesValueImpl(Isolate* isolate,
 Handle<JSObject> object,

                                        Handle<Object> value,
                                        uint32_t start_from, uint32_t length) {
     DCHECK(JSObject::PrototypeHasNoElements(isolate, *object));
    Handle<Map> original_map(object->map(), isolate);
     Handle<FixedArray> parameter_map(FixedArray::cast(object->elements()),
                                      isolate);
     bool search_for_hole = value->IsUndefined(isolate);
 
     for (uint32_t k = start_from; k < length; ++k) {
      DCHECK_EQ(object->map(), *original_map);
       uint32_t entry = GetEntryForIndexImpl(isolate, *object, *parameter_map, k,
                                             ALL_PROPERTIES);
       if (entry == kMaxUInt32) {
 if (search_for_hole) return Just(true);
 continue;
 }

 Handle<Object> element_k =
 Subclass::GetImpl(isolate, *parameter_map, entry);

 if (element_k->IsAccessorPair()) {
 LookupIterator it(isolate, object, k, LookupIterator::OWN);
        DCHECK(it.IsFound());
        DCHECK_EQ(it.state(), LookupIterator::ACCESSOR);
        ASSIGN_RETURN_ON_EXCEPTION_VALUE(isolate, element_k,
 Object::GetPropertyWithAccessor(&it),
 Nothing<bool>());

 if (value->SameValueZero(*element_k)) return Just(true);

 if (object->map() != *original_map) {
 return IncludesValueSlowPath(isolate, object, value, k + 1, length);
 }
 } else if (value->SameValueZero(*element_k)) {
 return Just(true);
 }
 }
 return Just(false);
 }
