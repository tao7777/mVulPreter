   InputMethodLibraryImpl()
      : ibus_controller_(NULL),
         should_launch_ime_(false),
         ime_connected_(false),
         defer_ime_startup_(false),
        enable_auto_ime_shutdown_(true),
        ibus_daemon_process_handle_(base::kNullProcessHandle),
#if !defined(TOUCH_UI)
        initialized_successfully_(false),
        candidate_window_controller_(NULL) {
#else
        initialized_successfully_(false) {
#endif
    notification_registrar_.Add(this, NotificationType::APP_TERMINATING,
                                NotificationService::AllSources());
  }

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
 
   virtual ~InputMethodLibraryImpl() {
    ibus_controller_->RemoveObserver(this);
   }
 
  virtual void AddObserver(InputMethodLibrary::Observer* observer) {
     if (!observers_.size()) {
       observer->FirstObserverIsAdded(this);
     }
     observers_.AddObserver(observer);
   }
 
  virtual void RemoveObserver(InputMethodLibrary::Observer* observer) {
     observers_.RemoveObserver(observer);
   }
 
  virtual input_method::InputMethodDescriptors* GetActiveInputMethods() {
    input_method::InputMethodDescriptors* result =
        new input_method::InputMethodDescriptors;
     for (size_t i = 0; i < active_input_method_ids_.size(); ++i) {
       const std::string& input_method_id = active_input_method_ids_[i];
      const input_method::InputMethodDescriptor* descriptor =
          input_method::GetInputMethodDescriptorFromId(
               input_method_id);
       if (descriptor) {
         result->push_back(*descriptor);
      } else {
        LOG(ERROR) << "Descriptor is not found for: " << input_method_id;
      }
    }
    if (result->empty()) {
      LOG(WARNING) << "No active input methods found.";
      result->push_back(input_method::GetFallbackInputMethodDescriptor());
    }
    return result;
   }
 
   virtual size_t GetNumActiveInputMethods() {
    scoped_ptr<input_method::InputMethodDescriptors> input_methods(
        GetActiveInputMethods());
     return input_methods->size();
   }
 
  virtual input_method::InputMethodDescriptors* GetSupportedInputMethods() {
     if (!initialized_successfully_) {
      input_method::InputMethodDescriptors* result =
          new input_method::InputMethodDescriptors;
       result->push_back(input_method::GetFallbackInputMethodDescriptor());
       return result;
     }
 
    return input_method::GetSupportedInputMethodDescriptors();
   }
 
   virtual void ChangeInputMethod(const std::string& input_method_id) {
    tentative_current_input_method_id_ = input_method_id;
     if (ibus_daemon_process_handle_ == base::kNullProcessHandle &&
        input_method::IsKeyboardLayout(input_method_id)) {
       ChangeCurrentInputMethodFromId(input_method_id);
    } else {
      StartInputMethodDaemon();
      if (!ChangeInputMethodViaIBus(input_method_id)) {
        VLOG(1) << "Failed to change the input method to " << input_method_id
                << " (deferring)";
      }
    }
  }

  virtual void SetImePropertyActivated(const std::string& key,
                                       bool activated) {
    if (!initialized_successfully_)
       return;
 
     DCHECK(!key.empty());
    ibus_controller_->SetImePropertyActivated(key, activated);
   }
 
   virtual bool InputMethodIsActivated(const std::string& input_method_id) {
    scoped_ptr<input_method::InputMethodDescriptors>
        active_input_method_descriptors(
            GetActiveInputMethods());
     for (size_t i = 0; i < active_input_method_descriptors->size(); ++i) {
       if (active_input_method_descriptors->at(i).id == input_method_id) {
         return true;
      }
    }
    return false;
  }
 
   virtual bool SetImeConfig(const std::string& section,
                             const std::string& config_name,
                            const input_method::ImeConfigValue& value) {
     if (section == language_prefs::kGeneralSectionName &&
         config_name == language_prefs::kPreloadEnginesConfigName &&
        value.type == input_method::ImeConfigValue::kValueTypeStringList) {
       active_input_method_ids_ = value.string_list_value;
     }
 
    MaybeStartInputMethodDaemon(section, config_name, value);

    const ConfigKeyType key = std::make_pair(section, config_name);
    current_config_values_[key] = value;
    if (ime_connected_) {
      pending_config_requests_[key] = value;
      FlushImeConfig();
    }

    MaybeStopInputMethodDaemon(section, config_name, value);
    MaybeChangeCurrentKeyboardLayout(section, config_name, value);
     return pending_config_requests_.empty();
   }
 
  virtual input_method::InputMethodDescriptor previous_input_method() const {
     if (previous_input_method_.id.empty()) {
       return input_method::GetFallbackInputMethodDescriptor();
     }
     return previous_input_method_;
   }
 
  virtual input_method::InputMethodDescriptor current_input_method() const {
     if (current_input_method_.id.empty()) {
       return input_method::GetFallbackInputMethodDescriptor();
     }
     return current_input_method_;
   }
 
  virtual const input_method::ImePropertyList& current_ime_properties() const {
     return current_ime_properties_;
   }
 
   virtual std::string GetKeyboardOverlayId(const std::string& input_method_id) {
     if (!initialized_successfully_)
       return "";
 
    return input_method::GetKeyboardOverlayId(input_method_id);
   }
 
  virtual void SendHandwritingStroke(
      const input_method::HandwritingStroke& stroke) {
     if (!initialized_successfully_)
       return;
    ibus_controller_->SendHandwritingStroke(stroke);
   }
 
   virtual void CancelHandwritingStrokes(int stroke_count) {
     if (!initialized_successfully_)
       return;
    ibus_controller_->CancelHandwriting(stroke_count);
   }
 
  private:
   bool ContainOnlyOneKeyboardLayout(
      const input_method::ImeConfigValue& value) {
    return (value.type == input_method::ImeConfigValue::kValueTypeStringList &&
             value.string_list_value.size() == 1 &&
            input_method::IsKeyboardLayout(
                 value.string_list_value[0]));
   }
 
   void MaybeStartInputMethodDaemon(const std::string& section,
                                    const std::string& config_name,
                                   const input_method::ImeConfigValue& value) {
     if (section == language_prefs::kGeneralSectionName &&
         config_name == language_prefs::kPreloadEnginesConfigName &&
        value.type == input_method::ImeConfigValue::kValueTypeStringList &&
         !value.string_list_value.empty()) {
      if (ContainOnlyOneKeyboardLayout(value) || defer_ime_startup_) {
        return;
      }

      const bool just_started = StartInputMethodDaemon();
      if (!just_started) {
        return;
      }

      if (tentative_current_input_method_id_.empty()) {
        tentative_current_input_method_id_ = current_input_method_.id;
      }

      if (std::find(value.string_list_value.begin(),
                    value.string_list_value.end(),
                    tentative_current_input_method_id_)
          == value.string_list_value.end()) {
        tentative_current_input_method_id_.clear();
      }
    }
  }

   void MaybeStopInputMethodDaemon(const std::string& section,
                                   const std::string& config_name,
                                  const input_method::ImeConfigValue& value) {
    if (section == language_prefs::kGeneralSectionName &&
        config_name == language_prefs::kPreloadEnginesConfigName &&
        ContainOnlyOneKeyboardLayout(value) &&
        enable_auto_ime_shutdown_) {
      StopInputMethodDaemon();
    }
  }
 
  void MaybeChangeCurrentKeyboardLayout(
      const std::string& section,
      const std::string& config_name,
      const input_method::ImeConfigValue& value) {
 
    if (section == language_prefs::kGeneralSectionName &&
        config_name == language_prefs::kPreloadEnginesConfigName &&
        ContainOnlyOneKeyboardLayout(value)) {
      ChangeCurrentInputMethodFromId(value.string_list_value[0]);
    }
  }

  bool ChangeInputMethodViaIBus(const std::string& input_method_id) {
    if (!initialized_successfully_)
      return false;

    std::string input_method_id_to_switch = input_method_id;

    if (!InputMethodIsActivated(input_method_id)) {
      scoped_ptr<input_method::InputMethodDescriptors> input_methods(
          GetActiveInputMethods());
       DCHECK(!input_methods->empty());
       if (!input_methods->empty()) {
         input_method_id_to_switch = input_methods->at(0).id;
        LOG(INFO) << "Can't change the current input method to "
                  << input_method_id << " since the engine is not preloaded. "
                  << "Switch to " << input_method_id_to_switch << " instead.";
       }
     }
 
    if (ibus_controller_->ChangeInputMethod(input_method_id_to_switch)) {
       return true;
     }
 
    LOG(ERROR) << "Can't switch input method to " << input_method_id_to_switch;
    return false;
  }

  void FlushImeConfig() {
    if (!initialized_successfully_)
      return;

    bool active_input_methods_are_changed = false;
    InputMethodConfigRequests::iterator iter =
        pending_config_requests_.begin();
     while (iter != pending_config_requests_.end()) {
       const std::string& section = iter->first.first;
       const std::string& config_name = iter->first.second;
      input_method::ImeConfigValue& value = iter->second;
 
       if (config_name == language_prefs::kPreloadEnginesConfigName &&
           !tentative_current_input_method_id_.empty()) {
        std::vector<std::string>::iterator engine_iter = std::find(
            value.string_list_value.begin(),
            value.string_list_value.end(),
            tentative_current_input_method_id_);
        if (engine_iter != value.string_list_value.end()) {
          std::rotate(value.string_list_value.begin(),
                      engine_iter,  // this becomes the new first element
                      value.string_list_value.end());
        } else {
          LOG(WARNING) << tentative_current_input_method_id_
                       << " is not in preload_engines: " << value.ToString();
        }
         tentative_current_input_method_id_.erase();
       }
 
      if (ibus_controller_->SetImeConfig(section, config_name, value)) {
         if (config_name == language_prefs::kPreloadEnginesConfigName) {
           active_input_methods_are_changed = true;
          VLOG(1) << "Updated preload_engines: " << value.ToString();
        }
        pending_config_requests_.erase(iter++);
      } else {
        break;
      }
    }

    if (active_input_methods_are_changed) {
       const size_t num_active_input_methods = GetNumActiveInputMethods();
      FOR_EACH_OBSERVER(InputMethodLibrary::Observer, observers_,
                         ActiveInputMethodsChanged(this,
                                                   current_input_method_,
                                                   num_active_input_methods));
     }
   }
 
  // IBusController override.
  virtual void OnCurrentInputMethodChanged(
      const input_method::InputMethodDescriptor& current_input_method) {
    if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
      LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    ChangeCurrentInputMethod(current_input_method);
   }
 
  // IBusController override.
  virtual void OnRegisterImeProperties(
      const input_method::ImePropertyList& prop_list) {
     if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
       LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    RegisterProperties(prop_list);
   }
 
  // IBusController override.
  virtual void OnUpdateImeProperty(
      const input_method::ImePropertyList& prop_list) {
     if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
       LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    UpdateProperty(prop_list);
   }
 
  // IBusController override.
  virtual void OnConnectionChange(bool connected) {
     if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
       LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    ime_connected_ = connected;
     if (connected) {
      pending_config_requests_.clear();
      pending_config_requests_.insert(
          current_config_values_.begin(),
          current_config_values_.end());
      FlushImeConfig();

      ChangeInputMethod(previous_input_method().id);
      ChangeInputMethod(current_input_method().id);
     }
   }
 
  void ChangeCurrentInputMethod(const input_method::InputMethodDescriptor&
                                new_input_method) {
     if (current_input_method_.id != new_input_method.id) {
       previous_input_method_ = current_input_method_;
       current_input_method_ = new_input_method;

      if (!input_method::SetCurrentKeyboardLayoutByName(
              current_input_method_.keyboard_layout)) {
        LOG(ERROR) << "Failed to change keyboard layout to "
                   << current_input_method_.keyboard_layout;
      }

      ObserverListBase<InputMethodLibrary::Observer>::Iterator it(observers_);
      InputMethodLibrary::Observer* first_observer = it.GetNext();
       if (first_observer) {
         first_observer->PreferenceUpdateNeeded(this,
                                                previous_input_method_,
                                               current_input_method_);
      }
    }

     const size_t num_active_input_methods = GetNumActiveInputMethods();
    FOR_EACH_OBSERVER(InputMethodLibrary::Observer, observers_,
                       InputMethodChanged(this,
                                          current_input_method_,
                                          num_active_input_methods));
  }

   void ChangeCurrentInputMethodFromId(const std::string& input_method_id) {
    const input_method::InputMethodDescriptor* descriptor =
        input_method::GetInputMethodDescriptorFromId(
             input_method_id);
     if (descriptor) {
       ChangeCurrentInputMethod(*descriptor);
    } else {
      LOG(ERROR) << "Descriptor is not found for: " << input_method_id;
    }
   }
 
  void RegisterProperties(const input_method::ImePropertyList& prop_list) {
     current_ime_properties_ = prop_list;
 
    FOR_EACH_OBSERVER(InputMethodLibrary::Observer, observers_,
                       PropertyListChanged(this,
                                           current_ime_properties_));
   }

  bool StartInputMethodDaemon() {
    should_launch_ime_ = true;
    return MaybeLaunchInputMethodDaemon();
   }
 
  void UpdateProperty(const input_method::ImePropertyList& prop_list) {
     for (size_t i = 0; i < prop_list.size(); ++i) {
       FindAndUpdateProperty(prop_list[i], &current_ime_properties_);
     }
 
    FOR_EACH_OBSERVER(InputMethodLibrary::Observer, observers_,
                       PropertyListChanged(this,
                                           current_ime_properties_));
   }

  bool LaunchInputMethodProcess(const std::string& command_line,
                                base::ProcessHandle* process_handle) {
    std::vector<std::string> argv;
    base::file_handle_mapping_vector fds_to_remap;
    base::ProcessHandle handle = base::kNullProcessHandle;

    base::SplitString(command_line, ' ', &argv);
    const bool result = base::LaunchApp(argv,
                                        fds_to_remap,  // no remapping
                                        false,  // wait
                                        &handle);
    if (!result) {
      LOG(ERROR) << "Could not launch: " << command_line;
      return false;
    }

    const base::ProcessId pid = base::GetProcId(handle);
    g_child_watch_add(pid,
                      reinterpret_cast<GChildWatchFunc>(OnImeShutdown),
                      this);

    *process_handle = handle;
    VLOG(1) << command_line << " (PID=" << pid << ") is started";
    return  true;
  }

  bool MaybeLaunchInputMethodDaemon() {
    if (!initialized_successfully_)
      return false;

    if (!should_launch_ime_) {
      return false;
    }

#if !defined(TOUCH_UI)
    if (!candidate_window_controller_.get()) {
      candidate_window_controller_.reset(new CandidateWindowController);
      if (!candidate_window_controller_->Init()) {
        LOG(WARNING) << "Failed to initialize the candidate window controller";
      }
    }
#endif

    if (ibus_daemon_process_handle_ != base::kNullProcessHandle) {
      return false;  // ibus-daemon is already running.
    }

    const std::string ibus_daemon_command_line =
        StringPrintf("%s --panel=disable --cache=none --restart --replace",
                     kIBusDaemonPath);
    if (!LaunchInputMethodProcess(
            ibus_daemon_command_line, &ibus_daemon_process_handle_)) {
      LOG(ERROR) << "Failed to launch " << ibus_daemon_command_line;
      return false;
    }
    return true;
  }

  static void OnImeShutdown(GPid pid,
                            gint status,
                            InputMethodLibraryImpl* library) {
    if (library->ibus_daemon_process_handle_ != base::kNullProcessHandle &&
        base::GetProcId(library->ibus_daemon_process_handle_) == pid) {
      library->ibus_daemon_process_handle_ = base::kNullProcessHandle;
    }

    library->MaybeLaunchInputMethodDaemon();
  }

  void StopInputMethodDaemon() {
    if (!initialized_successfully_)
      return;

     should_launch_ime_ = false;
     if (ibus_daemon_process_handle_ != base::kNullProcessHandle) {
       const base::ProcessId pid = base::GetProcId(ibus_daemon_process_handle_);
      if (!ibus_controller_->StopInputMethodProcess()) {
         LOG(ERROR) << "StopInputMethodProcess IPC failed. Sending SIGTERM to "
                    << "PID " << pid;
         base::KillProcess(ibus_daemon_process_handle_, -1, false /* wait */);
      }
      VLOG(1) << "ibus-daemon (PID=" << pid << ") is terminated";
      ibus_daemon_process_handle_ = base::kNullProcessHandle;
    }
  }

  void SetDeferImeStartup(bool defer) {
    VLOG(1) << "Setting DeferImeStartup to " << defer;
    defer_ime_startup_ = defer;
  }

  void SetEnableAutoImeShutdown(bool enable) {
    enable_auto_ime_shutdown_ = enable;
  }

  void Observe(NotificationType type,
               const NotificationSource& source,
               const NotificationDetails& details) {
    if (type.value == NotificationType::APP_TERMINATING) {
      notification_registrar_.RemoveAll();
      StopInputMethodDaemon();
#if !defined(TOUCH_UI)
      candidate_window_controller_.reset(NULL);
#endif
    }
  }
 
  input_method::IBusController* ibus_controller_;
  ObserverList<InputMethodLibrary::Observer> observers_;
 
  input_method::InputMethodDescriptor previous_input_method_;
  input_method::InputMethodDescriptor current_input_method_;
 
  input_method::ImePropertyList current_ime_properties_;
 
   typedef std::pair<std::string, std::string> ConfigKeyType;
  typedef std::map<ConfigKeyType,
        input_method::ImeConfigValue> InputMethodConfigRequests;
  InputMethodConfigRequests pending_config_requests_;

  InputMethodConfigRequests current_config_values_;

  NotificationRegistrar notification_registrar_;

  bool should_launch_ime_;
  bool ime_connected_;
  bool defer_ime_startup_;
  bool enable_auto_ime_shutdown_;
  std::string tentative_current_input_method_id_;

  base::ProcessHandle ibus_daemon_process_handle_;

  bool initialized_successfully_;

#if !defined(TOUCH_UI)
  scoped_ptr<CandidateWindowController> candidate_window_controller_;
#endif

  std::vector<std::string> active_input_method_ids_;

   DISALLOW_COPY_AND_ASSIGN(InputMethodLibraryImpl);
 };
