HRESULT CGaiaCredentialBase::ValidateOrCreateUser(
    const base::DictionaryValue* result,
    BSTR* domain,
    BSTR* username,
    BSTR* sid,
    BSTR* error_text) {
  LOGFN(INFO);
  DCHECK(domain);
  DCHECK(username);
  DCHECK(sid);
  DCHECK(error_text);
  DCHECK(sid);

  *error_text = nullptr;
  base::string16 local_password = GetDictString(result, kKeyPassword);

   wchar_t found_username[kWindowsUsernameBufferLength];
   wchar_t found_domain[kWindowsDomainBufferLength];
   wchar_t found_sid[kWindowsSidBufferLength];
   base::string16 gaia_id;
  MakeUsernameForAccount(
      result, &gaia_id, found_username, base::size(found_username),
      found_domain, base::size(found_domain), found_sid, base::size(found_sid));
 
  if (found_sid[0]) {
    HRESULT hr = ValidateExistingUser(found_username, found_domain, found_sid,
                                      error_text);

    if (FAILED(hr)) {
      LOGFN(ERROR) << "ValidateExistingUser hr=" << putHR(hr);
      return hr;
    }

    *username = ::SysAllocString(found_username);
    *domain = ::SysAllocString(found_domain);
    *sid = ::SysAllocString(found_sid);

    return S_OK;
  }

  DWORD cpus = 0;
  provider()->GetUsageScenario(&cpus);

  if (cpus == CPUS_UNLOCK_WORKSTATION) {
    *error_text = AllocErrorString(IDS_INVALID_UNLOCK_WORKSTATION_USER_BASE);
    return HRESULT_FROM_WIN32(ERROR_LOGON_TYPE_NOT_GRANTED);
  } else if (!CGaiaCredentialProvider::CanNewUsersBeCreated(
                 static_cast<CREDENTIAL_PROVIDER_USAGE_SCENARIO>(cpus))) {
    *error_text = AllocErrorString(IDS_ADD_USER_DISALLOWED_BASE);
    return HRESULT_FROM_WIN32(ERROR_LOGON_TYPE_NOT_GRANTED);
  }

  base::string16 local_fullname = GetDictString(result, kKeyFullname);
  base::string16 comment(GetStringResource(IDS_USER_ACCOUNT_COMMENT_BASE));
  HRESULT hr = CreateNewUser(
      OSUserManager::Get(), found_username, local_password.c_str(),
      local_fullname.c_str(), comment.c_str(),
      /*add_to_users_group=*/true, kMaxUsernameAttempts, username, sid);

  if (hr == HRESULT_FROM_WIN32(NERR_UserExists)) {
    LOGFN(ERROR) << "Could not find a new username based on desired username '"
                 << found_domain << "\\" << found_username
                 << "'. Maximum attempts reached.";
    *error_text = AllocErrorString(IDS_INTERNAL_ERROR_BASE);
    return hr;
  }

  *domain = ::SysAllocString(found_domain);

  return hr;
}
