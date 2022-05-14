test_function (char * (*my_asnprintf) (char *, size_t *, const char *, ...))
{
  char buf[8];
  int size;

  for (size = 0; size <= 8; size++)
    {
      size_t length = size;
      char *result = my_asnprintf (NULL, &length, "%d", 12345);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "12345") == 0);
      ASSERT (length == 5);
      free (result);
    }

  for (size = 0; size <= 8; size++)
    {
      size_t length;
      char *result;

      memcpy (buf, "DEADBEEF", 8);
      length = size;
      result = my_asnprintf (buf, &length, "%d", 12345);
       ASSERT (result != NULL);
       ASSERT (strcmp (result, "12345") == 0);
       ASSERT (length == 5);
      if (size < 5 + 1)
        ASSERT (result != buf);
      ASSERT (memcmp (buf + size, &"DEADBEEF"[size], 8 - size) == 0);
      if (result != buf)
        free (result);
    }

  /* Note: This test assumes IEEE 754 representation of 'double' floats.  */
  for (size = 0; size <= 8; size++)
    {
      size_t length;
      char *result;

      memcpy (buf, "DEADBEEF", 8);
      length = size;
      result = my_asnprintf (buf, &length, "%2.0f", 1.6314159265358979e+125);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "163141592653589790215729350939528493057529598899734151772468186268423257777068536614838678161083520756952076273094236944990208") == 0);
      ASSERT (length == 126);
      if (size < 126 + 1)
         ASSERT (result != buf);
       ASSERT (memcmp (buf + size, &"DEADBEEF"[size], 8 - size) == 0);
       if (result != buf)
        free (result);
    }
}
