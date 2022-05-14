 static Maybe<bool> IncludesValueImpl(Isolate* isolate,
 Handle<JSObject> receiver,
 Handle<Object> value,
 uint32_t start_from, uint32_t length) {
    DCHECK(JSObject::PrototypeHasNoElements(isolate, *receiver));
 bool search_for_hole = value->IsUndefined(isolate);

 if (!search_for_hole) {
 Maybe<bool> result = Nothing<bool>();
 if (DictionaryElementsAccessor::IncludesValueFastPath(
              isolate, receiver, value, start_from, length, &result)) {

         return result;
       }
     }
    Handle<Map> original_map(receiver->map(), isolate);
     Handle<SeededNumberDictionary> dictionary(
         SeededNumberDictionary::cast(receiver->elements()), isolate);
     for (uint32_t k = start_from; k < length; ++k) {
      DCHECK_EQ(receiver->map(), *original_map);
       int entry = dictionary->FindEntry(isolate, k);
       if (entry == SeededNumberDictionary::kNotFound) {
         if (search_for_hole) return Just(true);
 continue;
 }

 PropertyDetails details = GetDetailsImpl(*dictionary, entry);
 switch (details.kind()) {
 case kData: {
 Object* element_k = dictionary->ValueAt(entry);
 if (value->SameValueZero(element_k)) return Just(true);
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
 Nothing<bool>());

 if (value->SameValueZero(*element_k)) return Just(true);

 if (!JSObject::PrototypeHasNoElements(isolate, *receiver)) {
 return IncludesValueSlowPath(isolate, receiver, value, k + 1,
                                         length);
 }

 if (*dictionary == receiver->elements()) continue;

 if (receiver->GetElementsKind() != DICTIONARY_ELEMENTS) {
 if (receiver->map()->GetInitialElements() == receiver->elements()) {
 return Just(search_for_hole);
 }
 return IncludesValueSlowPath(isolate, receiver, value, k + 1,
                                         length);
 }
          dictionary = handle(
 SeededNumberDictionary::cast(receiver->elements()), isolate);
 break;
 }
 }
 }
 return Just(false);
 }
