  void OnDidAddMessageToConsole(int32_t,
                                const base::string16& message,
                                int32_t,
                                const base::string16&) {
    callback_.Run(message);
  }
