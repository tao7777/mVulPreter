   virtual std::string GetKeyboardOverlayId(const std::string& input_method_id) {
     if (!initialized_successfully_)
       return "";
 
    return chromeos::GetKeyboardOverlayId(input_method_id);
   }
