 bool IsValidURL(const GURL& url, PortPermission port_permission) {
   return url.is_valid() && url.SchemeIsHTTPOrHTTPS() &&
         (url.port().empty() || g_ignore_port_numbers ||
          (port_permission == ALLOW_NON_STANDARD_PORTS));
 }
