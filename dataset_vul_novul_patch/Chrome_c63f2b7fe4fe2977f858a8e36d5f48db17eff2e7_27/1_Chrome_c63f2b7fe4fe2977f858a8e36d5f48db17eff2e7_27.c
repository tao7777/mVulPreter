   virtual bool Speak(
       const std::string& utterance,
      const std::string& language,
      const std::string& gender,
      double rate,
      double pitch,
      double volume) {
     error_ = kNotSupportedError;
     return false;
   }
