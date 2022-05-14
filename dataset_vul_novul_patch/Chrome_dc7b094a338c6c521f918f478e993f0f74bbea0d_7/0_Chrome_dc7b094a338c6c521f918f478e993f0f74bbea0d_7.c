   bool ContainOnlyOneKeyboardLayout(
      const input_method::ImeConfigValue& value) {
    return (value.type == input_method::ImeConfigValue::kValueTypeStringList &&
             value.string_list_value.size() == 1 &&
            input_method::IsKeyboardLayout(
                 value.string_list_value[0]));
   }
