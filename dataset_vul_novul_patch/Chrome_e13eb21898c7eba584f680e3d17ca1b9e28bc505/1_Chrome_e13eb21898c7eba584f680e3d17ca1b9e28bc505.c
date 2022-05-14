bool UnpackOriginPermissions(const std::vector<std::string>& origins_input,
                             const PermissionSet& required_permissions,
                             const PermissionSet& optional_permissions,
                             bool allow_file_access,
                             UnpackPermissionSetResult* result,
                             std::string* error) {
  int user_script_schemes = UserScript::ValidUserScriptSchemes();
  int explicit_schemes = Extension::kValidHostPermissionSchemes;
  if (!allow_file_access) {
    user_script_schemes &= ~URLPattern::SCHEME_FILE;
     explicit_schemes &= ~URLPattern::SCHEME_FILE;
   }
 
   for (const auto& origin_str : origins_input) {
     URLPattern explicit_origin(explicit_schemes);
     URLPattern::ParseResult parse_result = explicit_origin.Parse(origin_str);
    if (URLPattern::ParseResult::kSuccess != parse_result) {
      *error = ErrorUtils::FormatErrorMessage(
          kInvalidOrigin, origin_str,
          URLPattern::GetParseResultString(parse_result));
       return false;
     }
 
     bool used_origin = false;
     if (required_permissions.explicit_hosts().ContainsPattern(
             explicit_origin)) {
      used_origin = true;
      result->required_explicit_hosts.AddPattern(explicit_origin);
    } else if (optional_permissions.explicit_hosts().ContainsPattern(
                   explicit_origin)) {
      used_origin = true;
      result->optional_explicit_hosts.AddPattern(explicit_origin);
    }
 
     URLPattern scriptable_origin(user_script_schemes);
     if (scriptable_origin.Parse(origin_str) ==
            URLPattern::ParseResult::kSuccess &&
        required_permissions.scriptable_hosts().ContainsPattern(
            scriptable_origin)) {
      used_origin = true;
      result->required_scriptable_hosts.AddPattern(scriptable_origin);
     }
 
     if (!used_origin)
      result->unlisted_hosts.AddPattern(explicit_origin);
  }

  return true;
}
