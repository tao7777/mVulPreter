  virtual InputMethodDescriptors* GetActiveInputMethods() {
    chromeos::InputMethodDescriptors* result =
        new chromeos::InputMethodDescriptors;
     for (size_t i = 0; i < active_input_method_ids_.size(); ++i) {
       const std::string& input_method_id = active_input_method_ids_[i];
      const InputMethodDescriptor* descriptor =
          chromeos::input_method::GetInputMethodDescriptorFromId(
               input_method_id);
       if (descriptor) {
         result->push_back(*descriptor);
      } else {
        LOG(ERROR) << "Descriptor is not found for: " << input_method_id;
      }
    }
    if (result->empty()) {
      LOG(WARNING) << "No active input methods found.";
      result->push_back(input_method::GetFallbackInputMethodDescriptor());
    }
    return result;
   }
