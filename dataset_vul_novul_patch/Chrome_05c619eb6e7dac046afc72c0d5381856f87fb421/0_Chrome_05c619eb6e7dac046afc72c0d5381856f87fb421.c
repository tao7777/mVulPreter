void ShellSurface::CreateShellSurfaceWidget(ui::WindowShowState show_state) {
  DCHECK(enabled());
  DCHECK(!widget_);

  views::Widget::InitParams params;
  params.type = views::Widget::InitParams::TYPE_WINDOW;
  params.ownership = views::Widget::InitParams::NATIVE_WIDGET_OWNS_WIDGET;
  params.delegate = this;
  params.shadow_type = views::Widget::InitParams::SHADOW_TYPE_NONE;
  params.opacity = views::Widget::InitParams::TRANSLUCENT_WINDOW;
  params.show_state = show_state;
   params.parent =
       ash::Shell::GetContainer(ash::Shell::GetPrimaryRootWindow(), container_);
   params.bounds = initial_bounds_;
  bool activatable = activatable_;
  // ShellSurfaces in system modal container are only activatable if input
  // region is non-empty. See OnCommitSurface() for more details.
  if (container_ == ash::kShellWindowId_SystemModalContainer)
    activatable &= !surface_->GetHitTestBounds().IsEmpty();
   params.activatable = activatable ? views::Widget::InitParams::ACTIVATABLE_YES
                                    : views::Widget::InitParams::ACTIVATABLE_NO;
 
  widget_ = new ShellSurfaceWidget(this);
  widget_->Init(params);

  widget_->set_movement_disabled(!initial_bounds_.IsEmpty());

  aura::Window* window = widget_->GetNativeWindow();
  window->SetName("ExoShellSurface");
  window->AddChild(surface_->window());
  window->SetEventTargeter(base::WrapUnique(new CustomWindowTargeter));
  SetApplicationId(window, &application_id_);
  SetMainSurface(window, surface_);

  window->AddObserver(this);
  ash::wm::GetWindowState(window)->AddObserver(this);

  if (parent_)
    wm::AddTransientChild(parent_, window);

  ash::wm::GetWindowState(window)->set_window_position_managed(
      ash::wm::ToWindowShowState(ash::wm::WINDOW_STATE_TYPE_AUTO_POSITIONED) ==
          show_state &&
      initial_bounds_.IsEmpty());

  views::FocusManager* focus_manager = widget_->GetFocusManager();
  for (const auto& entry : kCloseWindowAccelerators) {
    focus_manager->RegisterAccelerator(
        ui::Accelerator(entry.keycode, entry.modifiers),
        ui::AcceleratorManager::kNormalPriority, this);
  }

  pending_show_widget_ = true;
}
