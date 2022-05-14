   bool Init() {
     DCHECK(!initialized_successfully_) << "Already initialized";
 
    ibus_controller_ = input_method::IBusController::Create();
    // The observer should be added before Connect() so we can capture the
    // initial connection change.
    ibus_controller_->AddObserver(this);
    ibus_controller_->Connect();
 
     initialized_successfully_ = true;
     return true;
   }
