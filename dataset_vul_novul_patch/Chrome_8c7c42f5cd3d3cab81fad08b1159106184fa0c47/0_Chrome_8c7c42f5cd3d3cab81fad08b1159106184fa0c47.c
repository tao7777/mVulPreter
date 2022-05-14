ChromeGeolocationPermissionContext::ChromeGeolocationPermissionContext(
    Profile* profile)
    : profile_(profile),
      ALLOW_THIS_IN_INITIALIZER_LIST(geolocation_infobar_queue_controller_(
          new GeolocationInfoBarQueueController(
              base::Bind(
                  &ChromeGeolocationPermissionContext::NotifyPermissionSet,
                 base::Unretained(this)),
              profile))) {
 }
