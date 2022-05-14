 bool IsSystemModal(aura::Window* window) {
  return window->GetProperty(aura::client::kModalKey) == ui::MODAL_TYPE_SYSTEM;
 }
