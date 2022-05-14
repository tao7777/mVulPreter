 bool SetCookieFunction::RunImpl() {
   DictionaryValue* details;
   EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(0, &details));
 
   GURL url;
  if (!ParseUrl(details, &url))
       return false;
  if (!GetExtension()->HasHostPermission(url)) {
    error_ = ExtensionErrorUtils::FormatErrorMessage(
        keys::kNoHostPermissionsError, url.spec());
    return false;
  }
   std::string name;
   if (details->HasKey(keys::kNameKey)) {
     EXTENSION_FUNCTION_VALIDATE(details->GetString(keys::kNameKey, &name));
  }
  std::string value;
  if (details->HasKey(keys::kValueKey)) {
    EXTENSION_FUNCTION_VALIDATE(details->GetString(keys::kValueKey, &value));
  }
  std::string domain;
  if (details->HasKey(keys::kDomainKey)) {
    EXTENSION_FUNCTION_VALIDATE(details->GetString(keys::kDomainKey, &domain));
  }
  std::string path;
  if (details->HasKey(keys::kPathKey)) {
    EXTENSION_FUNCTION_VALIDATE(details->GetString(keys::kPathKey, &path));
  }
  bool secure = false;
  if (details->HasKey(keys::kSecureKey)) {
    EXTENSION_FUNCTION_VALIDATE(details->GetBoolean(keys::kSecureKey, &secure));
  }
  bool http_only = false;
  if (details->HasKey(keys::kHttpOnlyKey)) {
    EXTENSION_FUNCTION_VALIDATE(
        details->GetBoolean(keys::kHttpOnlyKey, &http_only));
  }
  base::Time expiration_time;
  if (details->HasKey(keys::kExpirationDateKey)) {
    Value* expiration_date_value;
    EXTENSION_FUNCTION_VALIDATE(details->Get(keys::kExpirationDateKey,
                                             &expiration_date_value));
    double expiration_date;
    if (expiration_date_value->IsType(Value::TYPE_INTEGER)) {
      int expiration_date_int;
      EXTENSION_FUNCTION_VALIDATE(
          expiration_date_value->GetAsInteger(&expiration_date_int));
      expiration_date = static_cast<double>(expiration_date_int);
    } else {
      EXTENSION_FUNCTION_VALIDATE(
          expiration_date_value->GetAsReal(&expiration_date));
    }
    expiration_time = base::Time::FromDoubleT(expiration_date);
  }

  net::CookieStore* cookie_store;
  if (!ParseCookieStore(details, &cookie_store, NULL))
    return false;
   DCHECK(cookie_store);
 
   if (!cookie_store->GetCookieMonster()->SetCookieWithDetails(
      url, name, value, domain, path, expiration_time, secure, http_only)) {
     error_ = ExtensionErrorUtils::FormatErrorMessage(
         keys::kCookieSetFailedError, name);
     return false;
  }
  return true;
 }
