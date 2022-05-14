 void BrowserActionsContainer::ViewHierarchyChanged(bool is_add,
                                                    views::View* parent,
                                                    views::View* child) {
   if (is_add && child == this) {
    ExtensionsService* extension_service = profile_->GetExtensionsService();
    if (!extension_service)
      return;  // The |extension_service| can be NULL in Incognito.
    for (size_t i = 0; i < extension_service->extensions()->size(); ++i)
      AddBrowserAction(extension_service->extensions()->at(i));
   }
 }
