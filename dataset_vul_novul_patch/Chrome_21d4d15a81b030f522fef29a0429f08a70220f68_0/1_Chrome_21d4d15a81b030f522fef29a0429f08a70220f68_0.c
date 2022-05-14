void GuestViewBase::RegisterGuestViewType(
    const std::string& view_type,
    const GuestCreationCallback& callback) {
  auto it = guest_view_registry.Get().find(view_type);
  DCHECK(it == guest_view_registry.Get().end());
  guest_view_registry.Get()[view_type] = callback;
}
