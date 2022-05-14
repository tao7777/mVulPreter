idna_strerror (Idna_rc rc)
{
  const char *p;

  bindtextdomain (PACKAGE, LOCALEDIR);

  switch (rc)
    {
    case IDNA_SUCCESS:
      p = _("Success");
      break;

    case IDNA_STRINGPREP_ERROR:
      p = _("String preparation failed");
      break;

    case IDNA_PUNYCODE_ERROR:
      p = _("Punycode failed");
      break;

    case IDNA_CONTAINS_NON_LDH:
      p = _("Non-digit/letter/hyphen in input");
      break;

    case IDNA_CONTAINS_MINUS:
      p = _("Forbidden leading or trailing minus sign (`-')");
      break;

    case IDNA_INVALID_LENGTH:
      p = _("Output would be too large or too small");
      break;

    case IDNA_NO_ACE_PREFIX:
      p = _("Input does not start with ACE prefix (`xn--')");
      break;

    case IDNA_ROUNDTRIP_VERIFY_ERROR:
      p = _("String not idempotent under ToASCII");
      break;

    case IDNA_CONTAINS_ACE_PREFIX:
      p = _("Input already contain ACE prefix (`xn--')");
       break;
 
     case IDNA_ICONV_ERROR:
      p = _("System iconv failed");
       break;
 
     case IDNA_MALLOC_ERROR:
      p = _("Cannot allocate memory");
      break;

    case IDNA_DLOPEN_ERROR:
      p = _("System dlopen failed");
      break;

    default:
      p = _("Unknown error");
      break;
    }

  return p;
}
