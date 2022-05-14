  virtual void AddObserver(Observer* observer) {
  virtual void AddObserver(InputMethodLibrary::Observer* observer) {
     if (!observers_.size()) {
       observer->FirstObserverIsAdded(this);
     }
     observers_.AddObserver(observer);
   }
