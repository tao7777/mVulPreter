 bool MatchFilter::MatchesCookie(
     const net::CookieMonster::CookieListPair& cookie_pair) {
   const net::CookieMonster::CanonicalCookie& cookie = cookie_pair.second;
  if (!MatchesString(keys::kNameKey, cookie.Name()))
      return false;
  if (!MatchesDomain(cookie_pair.first))
      return false;
  if (!MatchesString(keys::kPathKey, cookie.Path()))
      return false;
  if (!MatchesBoolean(keys::kSecureKey, cookie.IsSecure()))
      return false;
  if (!MatchesBoolean(keys::kSessionKey, !cookie.DoesExpire()))
      return false;
  return true;
 }
