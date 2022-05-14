void Shell::Init(const ShellInitParams& init_params) {
  in_mus_ = init_params.in_mus;
  if (!in_mus_) {
    native_cursor_manager_ = new AshNativeCursorManager;
#if defined(OS_CHROMEOS)
    cursor_manager_.reset(
        new CursorManager(make_scoped_ptr(native_cursor_manager_)));
#else
    cursor_manager_.reset(
        new ::wm::CursorManager(make_scoped_ptr(native_cursor_manager_)));
#endif
  }

  delegate_->PreInit();
  bool display_initialized = display_manager_->InitFromCommandLine();

  display_configuration_controller_.reset(new DisplayConfigurationController(
       display_manager_.get(), window_tree_host_manager_.get()));
 
 #if defined(OS_CHROMEOS)
  // When running as part of mash, OzonePlatform is not initialized in the
  // ash_sysui process. DisplayConfigurator will try to use OzonePlatform and
  // crash. Instead, mash can manually set default display size using
  // --ash-host-window-bounds flag.
  if (in_mus_)
    display_configurator_->set_configure_display(false);
   display_configurator_->Init(!gpu_support_->IsPanelFittingDisabled());
 
  chromeos::DBusThreadManager* dbus_thread_manager =
      chromeos::DBusThreadManager::Get();
  projecting_observer_.reset(
      new ProjectingObserver(dbus_thread_manager->GetPowerManagerClient()));
  display_configurator_->AddObserver(projecting_observer_.get());
  AddShellObserver(projecting_observer_.get());

  if (!display_initialized && base::SysInfo::IsRunningOnChromeOS()) {
    display_change_observer_.reset(new DisplayChangeObserver);
    display_configurator_->AddObserver(display_change_observer_.get());
    display_error_observer_.reset(new DisplayErrorObserver());
    display_configurator_->AddObserver(display_error_observer_.get());
    display_configurator_->set_state_controller(display_change_observer_.get());
    display_configurator_->set_mirroring_controller(display_manager_.get());
    display_configurator_->ForceInitialConfigure(
        delegate_->IsFirstRunAfterBoot() ? kChromeOsBootColor : 0);
    display_initialized = true;
  }
  display_color_manager_.reset(
      new DisplayColorManager(display_configurator_.get(), blocking_pool_));
#endif  // defined(OS_CHROMEOS)

  if (!display_initialized)
    display_manager_->InitDefaultDisplay();

  display_manager_->RefreshFontParams();

  views::FocusManagerFactory::Install(new AshFocusManagerFactory);

  aura::Env::GetInstance()->set_context_factory(init_params.context_factory);

  window_modality_controller_.reset(
      new ::wm::WindowModalityController(this));

  env_filter_.reset(new ::wm::CompoundEventFilter);
  AddPreTargetHandler(env_filter_.get());

  wm::AshFocusRules* focus_rules = new wm::AshFocusRules();

  ::wm::FocusController* focus_controller =
      new ::wm::FocusController(focus_rules);
  focus_client_.reset(focus_controller);
  activation_client_ = focus_controller;
  activation_client_->AddObserver(this);
  focus_cycler_.reset(new FocusCycler());

  screen_position_controller_.reset(new ScreenPositionController);

  window_tree_host_manager_->Start();
  window_tree_host_manager_->CreatePrimaryHost(
      ShellInitParamsToAshWindowTreeHostInitParams(init_params));
  aura::Window* root_window = window_tree_host_manager_->GetPrimaryRootWindow();
  target_root_window_ = root_window;

#if defined(OS_CHROMEOS)
  resolution_notification_controller_.reset(
      new ResolutionNotificationController);
#endif

  if (cursor_manager_)
    cursor_manager_->SetDisplay(gfx::Screen::GetScreen()->GetPrimaryDisplay());

  accelerator_controller_.reset(new AcceleratorController);
  maximize_mode_controller_.reset(new MaximizeModeController());

  AddPreTargetHandler(window_tree_host_manager_->input_method_event_handler());

#if defined(OS_CHROMEOS)
  magnifier_key_scroll_handler_ = MagnifierKeyScroller::CreateHandler();
  AddPreTargetHandler(magnifier_key_scroll_handler_.get());
  speech_feedback_handler_ = SpokenFeedbackToggler::CreateHandler();
  AddPreTargetHandler(speech_feedback_handler_.get());
#endif


  user_activity_detector_.reset(new ui::UserActivityDetector);

  overlay_filter_.reset(new OverlayEventFilter);
  AddPreTargetHandler(overlay_filter_.get());
  AddShellObserver(overlay_filter_.get());

  accelerator_filter_.reset(new ::wm::AcceleratorFilter(
      scoped_ptr<::wm::AcceleratorDelegate>(new AcceleratorDelegate),
      accelerator_controller_->accelerator_history()));
  AddPreTargetHandler(accelerator_filter_.get());

  event_transformation_handler_.reset(new EventTransformationHandler);
  AddPreTargetHandler(event_transformation_handler_.get());

  toplevel_window_event_handler_.reset(new ToplevelWindowEventHandler);

  system_gesture_filter_.reset(new SystemGestureEventFilter);
  AddPreTargetHandler(system_gesture_filter_.get());

  keyboard_metrics_filter_.reset(new KeyboardUMAEventFilter);
  AddPreTargetHandler(keyboard_metrics_filter_.get());

#if defined(OS_CHROMEOS)
  sticky_keys_controller_.reset(new StickyKeysController);
#endif

  lock_state_controller_.reset(new LockStateController);
  power_button_controller_.reset(new PowerButtonController(
      lock_state_controller_.get()));
#if defined(OS_CHROMEOS)
  power_button_controller_->OnDisplayModeChanged(
      display_configurator_->cached_displays());
#endif
  AddShellObserver(lock_state_controller_.get());

  drag_drop_controller_.reset(new DragDropController);
  partial_screenshot_controller_.reset(new PartialScreenshotController());
  mouse_cursor_filter_.reset(new MouseCursorEventFilter());
  PrependPreTargetHandler(mouse_cursor_filter_.get());

  visibility_controller_.reset(new AshVisibilityController);

  magnification_controller_.reset(
      MagnificationController::CreateInstance());
  mru_window_tracker_.reset(new MruWindowTracker(activation_client_,
                                                 focus_rules));

  partial_magnification_controller_.reset(
      new PartialMagnificationController());

  autoclick_controller_.reset(AutoclickController::CreateInstance());

  high_contrast_controller_.reset(new HighContrastController);
  video_detector_.reset(new VideoDetector);
  window_selector_controller_.reset(new WindowSelectorController());
  window_cycle_controller_.reset(new WindowCycleController());

  tooltip_controller_.reset(new views::corewm::TooltipController(
      scoped_ptr<views::corewm::Tooltip>(new views::corewm::TooltipAura)));
  AddPreTargetHandler(tooltip_controller_.get());

  event_client_.reset(new EventClientImpl);

  desktop_background_controller_.reset(
      new DesktopBackgroundController(blocking_pool_));
  user_wallpaper_delegate_.reset(delegate_->CreateUserWallpaperDelegate());

  session_state_delegate_.reset(delegate_->CreateSessionStateDelegate());
  accessibility_delegate_.reset(delegate_->CreateAccessibilityDelegate());
  new_window_delegate_.reset(delegate_->CreateNewWindowDelegate());
  media_delegate_.reset(delegate_->CreateMediaDelegate());

  resize_shadow_controller_.reset(new ResizeShadowController());
  shadow_controller_.reset(
      new ::wm::ShadowController(activation_client_));

  system_tray_notifier_.reset(new SystemTrayNotifier());

  system_tray_delegate_.reset(delegate()->CreateSystemTrayDelegate());
  DCHECK(system_tray_delegate_.get());

  locale_notification_controller_.reset(new LocaleNotificationController);

  system_tray_delegate_->Initialize();

#if defined(OS_CHROMEOS)
  logout_confirmation_controller_.reset(new LogoutConfirmationController(
      base::Bind(&SystemTrayDelegate::SignOut,
                 base::Unretained(system_tray_delegate_.get()))));

  touch_transformer_controller_.reset(new TouchTransformerController());
#endif  // defined(OS_CHROMEOS)

  keyboard_ui_ = init_params.keyboard_factory.is_null()
                     ? KeyboardUI::Create()
                     : init_params.keyboard_factory.Run();

  window_tree_host_manager_->InitHosts();

#if defined(OS_CHROMEOS)
  virtual_keyboard_controller_.reset(new VirtualKeyboardController);
#endif  // defined(OS_CHROMEOS)

  user_wallpaper_delegate_->InitializeWallpaper();

  if (cursor_manager_) {
    if (initially_hide_cursor_)
      cursor_manager_->HideCursor();
    cursor_manager_->SetCursor(ui::kCursorPointer);
  }

#if defined(OS_CHROMEOS)
  accelerator_controller_->SetBrightnessControlDelegate(
      scoped_ptr<BrightnessControlDelegate>(
          new system::BrightnessControllerChromeos));

  power_event_observer_.reset(new PowerEventObserver());
  user_activity_notifier_.reset(
      new ui::UserActivityPowerManagerNotifier(user_activity_detector_.get()));
  video_activity_notifier_.reset(
      new VideoActivityNotifier(video_detector_.get()));
  bluetooth_notification_controller_.reset(new BluetoothNotificationController);
  last_window_closed_logout_reminder_.reset(new LastWindowClosedLogoutReminder);
  screen_orientation_controller_.reset(new ScreenOrientationController());
#endif
  display_manager_->CreateMirrorWindowAsyncIfAny();

  FOR_EACH_OBSERVER(ShellObserver, observers_, OnShellInitialized());

  user_metrics_recorder_->OnShellInitialized();
}
