ExtensionsGuestViewMessageFilter::FrameNavigationHelper::GetGuestView() const {
  return MimeHandlerViewGuest::From(
             parent_site_instance_->GetProcess()->GetID(), guest_instance_id_)
      ->As<MimeHandlerViewGuest>();
}
