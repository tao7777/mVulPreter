  explicit ConsoleCallbackFilter(
      base::Callback<void(const base::string16&)> callback)
      : callback_(callback) {}
