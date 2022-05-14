   virtual ~InputMethodLibraryImpl() {
    ibus_controller_->RemoveObserver(this);
   }
