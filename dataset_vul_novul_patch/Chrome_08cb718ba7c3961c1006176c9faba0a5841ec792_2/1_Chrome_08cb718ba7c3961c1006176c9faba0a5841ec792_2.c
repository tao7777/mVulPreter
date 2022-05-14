base::string16 IDNToUnicodeWithAdjustments(
    base::StringPiece host, base::OffsetAdjuster::Adjustments* adjustments) {
  if (adjustments)
    adjustments->clear();
  base::string16 input16;
   input16.reserve(host.length());
   input16.insert(input16.end(), host.begin(), host.end());
 
   base::string16 out16;
  for (size_t component_start = 0, component_end;
       component_start < input16.length();
       component_start = component_end + 1) {
    component_end = input16.find('.', component_start);
    if (component_end == base::string16::npos)
      component_end = input16.length();  // For getting the last component.
    size_t component_length = component_end - component_start;
    size_t new_component_start = out16.length();
    bool converted_idn = false;
    if (component_end > component_start) {
       converted_idn =
           IDNToUnicodeOneComponent(input16.data() + component_start,
                                   component_length, &out16);
     }
     size_t new_component_length = out16.length() - new_component_start;
 
    if (converted_idn && adjustments) {
      adjustments->push_back(base::OffsetAdjuster::Adjustment(
          component_start, component_length, new_component_length));
    }

    if (component_end < input16.length())
      out16.push_back('.');
  }
  return out16;
}
