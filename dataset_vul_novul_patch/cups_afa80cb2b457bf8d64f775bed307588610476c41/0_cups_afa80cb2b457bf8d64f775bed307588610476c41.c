valid_host(cupsd_client_t *con)		/* I - Client connection */
{
  cupsd_alias_t	*a;			/* Current alias */
  cupsd_netif_t	*netif;			/* Current network interface */
  const char	*end;			/* End character */
  char		*ptr;			/* Pointer into host value */


 /*
  * Copy the Host: header for later use...
  */

  strlcpy(con->clientname, httpGetField(con->http, HTTP_FIELD_HOST),
          sizeof(con->clientname));
  if ((ptr = strrchr(con->clientname, ':')) != NULL && !strchr(ptr, ']'))
  {
    *ptr++ = '\0';
    con->clientport = atoi(ptr);
  }
  else
    con->clientport = con->serverport;

 /*
  * Then validate...
  */

  if (httpAddrLocalhost(httpGetAddress(con->http)))
  {
   /*
    * Only allow "localhost" or the equivalent IPv4 or IPv6 numerical
    * addresses when accessing CUPS via the loopback interface...
    */
 
     return (!_cups_strcasecmp(con->clientname, "localhost") ||
 	    !_cups_strcasecmp(con->clientname, "localhost.") ||
             !strcmp(con->clientname, "127.0.0.1") ||
 	    !strcmp(con->clientname, "[::1]"));
   }

#if defined(HAVE_DNSSD) || defined(HAVE_AVAHI)
 /*
  * Check if the hostname is something.local (Bonjour); if so, allow it.
  */

  if ((end = strrchr(con->clientname, '.')) != NULL && end > con->clientname &&
      !end[1])
  {
   /*
    * "." on end, work back to second-to-last "."...
    */

    for (end --; end > con->clientname && *end != '.'; end --);
  }

  if (end && (!_cups_strcasecmp(end, ".local") ||
	      !_cups_strcasecmp(end, ".local.")))
    return (1);
#endif /* HAVE_DNSSD || HAVE_AVAHI */

 /*
  * Check if the hostname is an IP address...
  */

  if (isdigit(con->clientname[0] & 255) || con->clientname[0] == '[')
  {
   /*
    * Possible IPv4/IPv6 address...
    */

    http_addrlist_t *addrlist;		/* List of addresses */


    if ((addrlist = httpAddrGetList(con->clientname, AF_UNSPEC, NULL)) != NULL)
    {
     /*
      * Good IPv4/IPv6 address...
      */

      httpAddrFreeList(addrlist);
      return (1);
    }
  }

 /*
  * Check for (alias) name matches...
  */

  for (a = (cupsd_alias_t *)cupsArrayFirst(ServerAlias);
       a;
       a = (cupsd_alias_t *)cupsArrayNext(ServerAlias))
  {
   /*
    * "ServerAlias *" allows all host values through...
    */

    if (!strcmp(a->name, "*"))
      return (1);

    if (!_cups_strncasecmp(con->clientname, a->name, a->namelen))
    {
     /*
      * Prefix matches; check the character at the end - it must be "." or nul.
      */

      end = con->clientname + a->namelen;

      if (!*end || (*end == '.' && !end[1]))
        return (1);
    }
  }

#if defined(HAVE_DNSSD) || defined(HAVE_AVAHI)
  for (a = (cupsd_alias_t *)cupsArrayFirst(DNSSDAlias);
       a;
       a = (cupsd_alias_t *)cupsArrayNext(DNSSDAlias))
  {
   /*
    * "ServerAlias *" allows all host values through...
    */

    if (!strcmp(a->name, "*"))
      return (1);

    if (!_cups_strncasecmp(con->clientname, a->name, a->namelen))
    {
     /*
      * Prefix matches; check the character at the end - it must be "." or nul.
      */

      end = con->clientname + a->namelen;

      if (!*end || (*end == '.' && !end[1]))
        return (1);
    }
  }
#endif /* HAVE_DNSSD || HAVE_AVAHI */

 /*
  * Check for interface hostname matches...
  */

  for (netif = (cupsd_netif_t *)cupsArrayFirst(NetIFList);
       netif;
       netif = (cupsd_netif_t *)cupsArrayNext(NetIFList))
  {
    if (!_cups_strncasecmp(con->clientname, netif->hostname, netif->hostlen))
    {
     /*
      * Prefix matches; check the character at the end - it must be "." or nul.
      */

      end = con->clientname + netif->hostlen;

      if (!*end || (*end == '.' && !end[1]))
        return (1);
    }
  }

  return (0);
}
