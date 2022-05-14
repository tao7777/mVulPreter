   virtual bool Speak(
      int utterance_id,
       const std::string& utterance,
      const std::string& lang,
      const UtteranceContinuousParameters& params) {
     error_ = kNotSupportedError;
     return false;
   }
