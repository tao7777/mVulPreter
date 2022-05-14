void GuestViewBase::RegisterGuestViewTypes() {
  AppViewGuest::Register();
  ExtensionOptionsGuest::Register();
  ExtensionViewGuest::Register();
  MimeHandlerViewGuest::Register();
  SurfaceWorkerGuest::Register();
  WebViewGuest::Register();
}
