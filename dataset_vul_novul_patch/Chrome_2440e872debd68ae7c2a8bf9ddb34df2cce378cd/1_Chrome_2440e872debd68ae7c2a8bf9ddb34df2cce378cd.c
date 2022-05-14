 void MakeUsernameForAccount(const base::DictionaryValue* result,
                             base::string16* gaia_id,
                             wchar_t* username,
                             DWORD username_length,
                             wchar_t* domain,
                             DWORD domain_length,
                             wchar_t* sid,
                            DWORD sid_length) {
   DCHECK(gaia_id);
   DCHECK(username);
   DCHECK(domain);
   DCHECK(sid);
 
   *gaia_id = GetDictString(result, kKeyId);
  HRESULT hr = GetSidFromId(*gaia_id, sid, sid_length);
  if (SUCCEEDED(hr)) {
    hr = OSUserManager::Get()->FindUserBySID(sid, username, username_length,
                                             domain, domain_length);
    if (SUCCEEDED(hr))
      return;
  }
  LOGFN(INFO) << "No existing user found associated to gaia id:" << *gaia_id;
  wcscpy_s(domain, domain_length, OSUserManager::GetLocalDomain().c_str());
  username[0] = 0;
  sid[0] = 0;

  base::string16 os_username = GetDictString(result, kKeyEmail);
  std::transform(os_username.begin(), os_username.end(), os_username.begin(),
                 ::tolower);
  base::string16::size_type at = os_username.find(L"@gmail.com");
  if (at == base::string16::npos)
    at = os_username.find(L"@googlemail.com");
  if (at != base::string16::npos) {
    os_username.resize(at);
   } else {
     std::string username_utf8 =
        gaia::SanitizeEmail(base::UTF16ToUTF8(os_username));

    size_t tld_length =
        net::registry_controlled_domains::GetCanonicalHostRegistryLength(
            gaia::ExtractDomainName(username_utf8),
            net::registry_controlled_domains::EXCLUDE_UNKNOWN_REGISTRIES,
            net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES);

    if (tld_length > 0) {
      username_utf8.resize(username_utf8.length() - tld_length - 1);
      os_username = base::UTF8ToUTF16(username_utf8);
    }
  }

  if (os_username.size() > kWindowsUsernameBufferLength - 1)
    os_username.resize(kWindowsUsernameBufferLength - 1);

  for (auto& c : os_username) {
    if (wcschr(L"@\\[]:|<>+=;?*", c) != nullptr || c < 32)
      c = L'_';
  }

  wcscpy_s(username, username_length, os_username.c_str());
}
