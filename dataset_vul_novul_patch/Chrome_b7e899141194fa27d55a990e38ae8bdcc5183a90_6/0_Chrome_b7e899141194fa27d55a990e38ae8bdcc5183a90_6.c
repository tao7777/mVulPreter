net::CookieMonster::CookieList GetCookieListFromStore(
    net::CookieStore* cookie_store, const GURL& url) {
  DCHECK(cookie_store);
  net::CookieMonster* monster = cookie_store->GetCookieMonster();
   if (!url.is_empty()) {
     DCHECK(url.is_valid());
     return monster->GetAllCookiesForURL(url);
   }
  return monster->GetAllCookies();
 }
