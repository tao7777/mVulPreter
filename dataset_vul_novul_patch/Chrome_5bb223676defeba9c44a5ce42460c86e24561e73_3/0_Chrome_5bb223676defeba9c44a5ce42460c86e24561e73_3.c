 ExtensionsGuestViewMessageFilter::ExtensionsGuestViewMessageFilter(
     int render_process_id,
    BrowserContext* context)
    : GuestViewMessageFilter(kFilteredMessageClasses,
                             base::size(kFilteredMessageClasses),
                              render_process_id,
                              context),
       content::BrowserAssociatedInterface<mojom::GuestView>(this, this) {
 }
