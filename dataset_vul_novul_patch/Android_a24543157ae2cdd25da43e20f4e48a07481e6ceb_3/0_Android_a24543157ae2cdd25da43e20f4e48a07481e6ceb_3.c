 static Maybe<int64_t> IndexOfValueImpl(Isolate* isolate,
 Handle<JSObject> receiver,
 Handle<Object> value,

                                          uint32_t start_from, uint32_t length) {
     DCHECK(JSObject::PrototypeHasNoElements(isolate, *receiver));
 
    Handle<Map> original_map(receiver->map(), isolate);
     Handle<SeededNumberDictionary> dictionary(
         SeededNumberDictionary::cast(receiver->elements()), isolate);
     for (uint32_t k = start_from; k < length; ++k) {
      DCHECK_EQ(receiver->map(), *original_map);
       int entry = dictionary->FindEntry(isolate, k);
       if (entry == SeededNumberDictionary::kNotFound) {
         continue;
 }

 PropertyDetails details = GetDetailsImpl(*dictionary, entry);
 switch (details.kind()) {
 case kData: {
 Object* element_k = dictionary->ValueAt(entry);
 if (value->StrictEquals(element_k)) {
 return Just<int64_t>(k);
 }
 break;
 }
 case kAccessor: {
 LookupIterator it(isolate, receiver, k,
 LookupIterator::OWN_SKIP_INTERCEPTOR);
          DCHECK(it.IsFound());
          DCHECK_EQ(it.state(), LookupIterator::ACCESSOR);
 Handle<Object> element_k;

          ASSIGN_RETURN_ON_EXCEPTION_VALUE(
              isolate, element_k, JSObject::GetPropertyWithAccessor(&it),
 Nothing<int64_t>());

 if (value->StrictEquals(*element_k)) return Just<int64_t>(k);

 if (!JSObject::PrototypeHasNoElements(isolate, *receiver)) {
 return IndexOfValueSlowPath(isolate, receiver, value, k + 1,
                                        length);
 }

 if (*dictionary == receiver->elements()) continue;

 if (receiver->GetElementsKind() != DICTIONARY_ELEMENTS) {
 return IndexOfValueSlowPath(isolate, receiver, value, k + 1,
                                        length);
 }
          dictionary = handle(
 SeededNumberDictionary::cast(receiver->elements()), isolate);
 break;
 }
 }
 }
 return Just<int64_t>(-1);
 }
