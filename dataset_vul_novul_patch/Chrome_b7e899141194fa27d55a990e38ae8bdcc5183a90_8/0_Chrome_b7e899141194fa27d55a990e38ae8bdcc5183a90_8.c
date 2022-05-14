 bool MatchFilter::MatchesCookie(
     const net::CookieMonster::CookieListPair& cookie_pair) {
   const net::CookieMonster::CanonicalCookie& cookie = cookie_pair.second;
  return
      MatchesString(keys::kNameKey, cookie.Name()) &&
      MatchesDomain(cookie_pair.first) &&
      MatchesString(keys::kPathKey, cookie.Path()) &&
      MatchesBoolean(keys::kSecureKey, cookie.IsSecure()) &&
      MatchesBoolean(keys::kSessionKey, !cookie.DoesExpire());
 }
