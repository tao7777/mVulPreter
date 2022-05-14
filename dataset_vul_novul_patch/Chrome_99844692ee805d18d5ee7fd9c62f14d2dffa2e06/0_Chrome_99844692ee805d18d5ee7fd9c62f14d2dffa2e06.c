void InterstitialPage::Observe(NotificationType type,
                               const NotificationSource& source,
                               const NotificationDetails& details) {
  switch (type.value) {
    case NotificationType::NAV_ENTRY_PENDING:
       Disable();
       TakeActionOnResourceDispatcher(CANCEL);
       break;
     case NotificationType::RENDER_WIDGET_HOST_DESTROYED:
      if (action_taken_ == NO_ACTION) {
        RenderViewHost* rvh = Source<RenderViewHost>(source).ptr();
        DCHECK(rvh->process()->id() == original_child_id_ &&
               rvh->routing_id() == original_rvh_id_);
        TakeActionOnResourceDispatcher(CANCEL);
      }
      break;
    case NotificationType::TAB_CONTENTS_DESTROYED:
    case NotificationType::NAV_ENTRY_COMMITTED:
      if (action_taken_ == NO_ACTION) {
        DontProceed();
      } else {
        Hide();
      }
      break;
    default:
      NOTREACHED();
  }
}
