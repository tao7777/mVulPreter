  void ClearActiveTab() {
  void ClearActiveTab(const Extension& extension) {
    extension.permissions_data()->ClearTabSpecificPermissions(kTabId);
   }
