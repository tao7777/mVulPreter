 Sample::const_iterator FindFirstFrameWithinFunction(
     const Sample& sample,
    const void* function_address,
    int function_size) {
  function_address = MaybeFixupFunctionAddressForILT(function_address);
   for (auto it = sample.begin(); it != sample.end(); ++it) {
    if ((reinterpret_cast<const void*>(it->instruction_pointer) >=
         function_address) &&
        (reinterpret_cast<const void*>(it->instruction_pointer) <
         (static_cast<const unsigned char*>(function_address) + function_size)))
       return it;
   }
   return sample.end();
}
