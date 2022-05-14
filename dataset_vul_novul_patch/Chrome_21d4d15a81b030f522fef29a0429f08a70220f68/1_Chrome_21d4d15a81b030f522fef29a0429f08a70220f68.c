GuestViewBase* GuestViewBase::Create(
    content::WebContents* owner_web_contents,
    const std::string& view_type) {
  if (guest_view_registry.Get().empty())
    RegisterGuestViewTypes();
  auto it = guest_view_registry.Get().find(view_type);
  if (it == guest_view_registry.Get().end()) {
    NOTREACHED();
    return nullptr;
  }
  return it->second.Run(owner_web_contents);
}
