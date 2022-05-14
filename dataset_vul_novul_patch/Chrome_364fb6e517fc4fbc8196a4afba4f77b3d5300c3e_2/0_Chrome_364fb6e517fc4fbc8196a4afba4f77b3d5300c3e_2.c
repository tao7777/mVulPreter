  virtual ~MockInputMethodObserver() {
  virtual ~SimpleMockInputMethodObserver() {
  }
  void Reset() {
    on_caret_bounds_changed_ = 0;
    on_input_locale_changed_ = 0;
  }
  size_t on_caret_bounds_changed() const {
    return on_caret_bounds_changed_;
  }
  size_t on_input_locale_changed() const {
    return on_input_locale_changed_;
   }
