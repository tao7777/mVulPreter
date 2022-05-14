 bool IsSystemModal(aura::Window* window) {
  return window->transient_parent() &&
      window->GetProperty(aura::client::kModalKey) == ui::MODAL_TYPE_SYSTEM;
 }
