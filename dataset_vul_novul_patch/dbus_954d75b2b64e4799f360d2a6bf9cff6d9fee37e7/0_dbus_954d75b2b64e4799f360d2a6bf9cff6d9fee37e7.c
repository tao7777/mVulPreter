int _dbus_printf_string_upper_bound (const char *format,
                                     va_list args)
{
  /* MSVCRT's vsnprintf semantics are a bit different */
   char buf[1024];
   int bufsize;
   int len;
  va_list args_copy;
 
   bufsize = sizeof (buf);
  DBUS_VA_COPY (args_copy, args);
  len = _vsnprintf (buf, bufsize - 1, format, args_copy);
  va_end (args_copy);
 
   while (len == -1) /* try again */
     {

      p = malloc (bufsize);

      if (p == NULL)
        return -1;

       if (p == NULL)
         return -1;
 
      DBUS_VA_COPY (args_copy, args);
      len = _vsnprintf (p, bufsize - 1, format, args_copy);
      va_end (args_copy);
       free (p);
     }
 * Returns the UTF-16 form of a UTF-8 string. The result should be
 * freed with dbus_free() when no longer needed.
 *
 * @param str the UTF-8 string
 * @param error return location for error code
 */
wchar_t *
_dbus_win_utf8_to_utf16 (const char *str,
                         DBusError  *error)
{
  DBusString s;
  int n;
  wchar_t *retval;

  _dbus_string_init_const (&s, str);

  if (!_dbus_string_validate_utf8 (&s, 0, _dbus_string_get_length (&s)))
    {
      dbus_set_error_const (error, DBUS_ERROR_FAILED, "Invalid UTF-8");
      return NULL;
    }

  n = MultiByteToWideChar (CP_UTF8, 0, str, -1, NULL, 0);

  if (n == 0)
    {
      _dbus_win_set_error_from_win_error (error, GetLastError ());
      return NULL;
    }

  retval = dbus_new (wchar_t, n);

  if (!retval)
    {
      _DBUS_SET_OOM (error);
      return NULL;
    }

  if (MultiByteToWideChar (CP_UTF8, 0, str, -1, retval, n) != n)
    {
      dbus_free (retval);
      dbus_set_error_const (error, DBUS_ERROR_FAILED, "MultiByteToWideChar inconsistency");
      return NULL;
    }

  return retval;
}

/**
 * Returns the UTF-8 form of a UTF-16 string. The result should be
 * freed with dbus_free() when no longer needed.
 *
 * @param str the UTF-16 string
 * @param error return location for error code
 */
char *
_dbus_win_utf16_to_utf8 (const wchar_t *str,
                         DBusError     *error)
{
  int n;
  char *retval;

  n = WideCharToMultiByte (CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);

  if (n == 0)
    {
      _dbus_win_set_error_from_win_error (error, GetLastError ());
      return NULL;
    }

  retval = dbus_malloc (n);

  if (!retval)
    {
      _DBUS_SET_OOM (error);
      return NULL;
    }

  if (WideCharToMultiByte (CP_UTF8, 0, str, -1, retval, n, NULL, NULL) != n)
    {
      dbus_free (retval);
      dbus_set_error_const (error, DBUS_ERROR_FAILED, "WideCharToMultiByte inconsistency");
      return NULL;
    }

  return retval;
}






/************************************************************************
 
 
 ************************************************************************/

dbus_bool_t
_dbus_win_account_to_sid (const wchar_t *waccount,
                          void      	 **ppsid,
                          DBusError 	  *error)
{
  dbus_bool_t retval = FALSE;
  DWORD sid_length, wdomain_length;
  SID_NAME_USE use;
  wchar_t *wdomain;

  *ppsid = NULL;

  sid_length = 0;
  wdomain_length = 0;
  if (!LookupAccountNameW (NULL, waccount, NULL, &sid_length,
                           NULL, &wdomain_length, &use) &&
      GetLastError () != ERROR_INSUFFICIENT_BUFFER)
    {
      _dbus_win_set_error_from_win_error (error, GetLastError ());
      return FALSE;
    }

  *ppsid = dbus_malloc (sid_length);
  if (!*ppsid)
    {
      _DBUS_SET_OOM (error);
      return FALSE;
    }

  wdomain = dbus_new (wchar_t, wdomain_length);
  if (!wdomain)
    {
      _DBUS_SET_OOM (error);
      goto out1;
    }

  if (!LookupAccountNameW (NULL, waccount, (PSID) *ppsid, &sid_length,
                           wdomain, &wdomain_length, &use))
    {
      _dbus_win_set_error_from_win_error (error, GetLastError ());
      goto out2;
    }

  if (!IsValidSid ((PSID) *ppsid))
    {
      dbus_set_error_const (error, DBUS_ERROR_FAILED, "Invalid SID");
      goto out2;
    }

  retval = TRUE;

out2:
  dbus_free (wdomain);
out1:
  if (!retval)
    {
      dbus_free (*ppsid);
      *ppsid = NULL;
    }

  return retval;
}

/** @} end of sysdeps-win */
