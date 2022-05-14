   bool ContainOnlyOneKeyboardLayout(
      const ImeConfigValue& value) {
    return (value.type == ImeConfigValue::kValueTypeStringList &&
             value.string_list_value.size() == 1 &&
            chromeos::input_method::IsKeyboardLayout(
                 value.string_list_value[0]));
   }
