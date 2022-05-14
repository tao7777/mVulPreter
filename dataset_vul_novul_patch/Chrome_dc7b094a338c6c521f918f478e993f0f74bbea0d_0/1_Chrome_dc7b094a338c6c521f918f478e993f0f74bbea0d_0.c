  virtual void AddObserver(Observer* observer) {
     if (!observers_.size()) {
       observer->FirstObserverIsAdded(this);
     }
     observers_.AddObserver(observer);
   }
