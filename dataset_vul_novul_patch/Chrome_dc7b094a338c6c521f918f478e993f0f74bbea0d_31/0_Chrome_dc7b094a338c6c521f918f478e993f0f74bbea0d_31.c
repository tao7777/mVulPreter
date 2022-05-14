  virtual void SetImePropertyActivated(const std::string& key,
                                       bool activated) {
    if (!initialized_successfully_)
       return;
 
     DCHECK(!key.empty());
    ibus_controller_->SetImePropertyActivated(key, activated);
   }
