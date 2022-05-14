bool DoCanonicalizePathComponent(const CHAR* source,
                                  const Component& component,
                                  char separator,
                                  CanonOutput* output,
                                  Component* new_component) {
  bool success = true;
   if (component.is_valid()) {
     if (separator)
       output->push_back(separator);
    new_component->begin = output->length();
    int end = component.end();
     for (int i = component.begin; i < end; i++) {
       UCHAR uch = static_cast<UCHAR>(source[i]);
       if (uch < 0x20 || uch >= 0x80)
        success &= AppendUTF8EscapedChar(source, &i, end, output);
       else
         output->push_back(static_cast<char>(uch));
     }
    new_component->len = output->length() - new_component->begin;
  } else {
     new_component->reset();
   }
  return success;
 }
