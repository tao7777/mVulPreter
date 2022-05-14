 scoped_refptr<PermissionSet> UnpackPermissionSet(
    const Permissions& permissions, std::string* error) {
   APIPermissionSet apis;
   std::vector<std::string>* permissions_list = permissions.permissions.get();
   if (permissions_list) {
    PermissionsInfo* info = PermissionsInfo::GetInstance();
    for (std::vector<std::string>::iterator it = permissions_list->begin();
        it != permissions_list->end(); ++it) {
      if (it->find(kDelimiter) != std::string::npos) {
        size_t delimiter = it->find(kDelimiter);
        std::string permission_name = it->substr(0, delimiter);
        std::string permission_arg = it->substr(delimiter + 1);

        scoped_ptr<base::Value> permission_json(
            base::JSONReader::Read(permission_arg));
        if (!permission_json.get()) {
          *error = ErrorUtils::FormatErrorMessage(kInvalidParameter, *it);
          return NULL;
        }

        APIPermission* permission = NULL;

        const APIPermissionInfo* bluetooth_device_permission_info =
            info->GetByID(APIPermission::kBluetoothDevice);
        const APIPermissionInfo* usb_device_permission_info =
            info->GetByID(APIPermission::kUsbDevice);
        if (permission_name == bluetooth_device_permission_info->name()) {
          permission = new BluetoothDevicePermission(
              bluetooth_device_permission_info);
        } else if (permission_name == usb_device_permission_info->name()) {
          permission = new UsbDevicePermission(usb_device_permission_info);
        } else {
          *error = kUnsupportedPermissionId;
          return NULL;
        }

        CHECK(permission);
        if (!permission->FromValue(permission_json.get())) {
          *error = ErrorUtils::FormatErrorMessage(kInvalidParameter, *it);
          return NULL;
        }
        apis.insert(permission);
      } else {
        const APIPermissionInfo* permission_info = info->GetByName(*it);
        if (!permission_info) {
          *error = ErrorUtils::FormatErrorMessage(
              kUnknownPermissionError, *it);
          return NULL;
        }
        apis.insert(permission_info->id());
      }
    }
  }

  URLPatternSet origins;
   if (permissions.origins.get()) {
     for (std::vector<std::string>::iterator it = permissions.origins->begin();
         it != permissions.origins->end(); ++it) {
      URLPattern origin(Extension::kValidHostPermissionSchemes);
       URLPattern::ParseResult parse_result = origin.Parse(*it);
       if (URLPattern::PARSE_SUCCESS != parse_result) {
         *error = ErrorUtils::FormatErrorMessage(
            kInvalidOrigin,
            *it,
            URLPattern::GetParseResultString(parse_result));
        return NULL;
      }
      origins.AddPattern(origin);
    }
  }

  return scoped_refptr<PermissionSet>(
      new PermissionSet(apis, origins, URLPatternSet()));
}
