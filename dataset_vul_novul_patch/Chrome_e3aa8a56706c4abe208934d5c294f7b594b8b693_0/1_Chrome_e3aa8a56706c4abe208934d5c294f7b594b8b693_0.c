UsbChooserContext::UsbChooserContext(Profile* profile)
    : ChooserContextBase(profile,
                         CONTENT_SETTINGS_TYPE_USB_GUARD,
                          CONTENT_SETTINGS_TYPE_USB_CHOOSER_DATA),
       is_incognito_(profile->IsOffTheRecord()),
       client_binding_(this),
      weak_factory_(this) {}
