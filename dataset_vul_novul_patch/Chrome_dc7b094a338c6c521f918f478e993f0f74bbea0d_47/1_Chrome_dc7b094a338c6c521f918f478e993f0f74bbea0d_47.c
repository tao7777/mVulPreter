InputMethodDescriptors* ChromeOSGetSupportedInputMethodDescriptors() {
  InputMethodDescriptors* input_methods = new InputMethodDescriptors;
  for (size_t i = 0; i < arraysize(chromeos::kIBusEngines); ++i) {
    if (InputMethodIdIsWhitelisted(chromeos::kIBusEngines[i].name)) {
      input_methods->push_back(chromeos::CreateInputMethodDescriptor(
          chromeos::kIBusEngines[i].name,
          chromeos::kIBusEngines[i].longname,
          chromeos::kIBusEngines[i].layout,
          chromeos::kIBusEngines[i].language));
    }
   }
  return input_methods;
}
