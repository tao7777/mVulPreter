stringprep_strerror (Stringprep_rc rc)
{
  const char *p;

  bindtextdomain (PACKAGE, LOCALEDIR);

  switch (rc)
    {
    case STRINGPREP_OK:
      p = _("Success");
      break;

    case STRINGPREP_CONTAINS_UNASSIGNED:
      p = _("Forbidden unassigned code points in input");
      break;

    case STRINGPREP_CONTAINS_PROHIBITED:
      p = _("Prohibited code points in input");
      break;

    case STRINGPREP_BIDI_BOTH_L_AND_RAL:
      p = _("Conflicting bidirectional properties in input");
      break;

    case STRINGPREP_BIDI_LEADTRAIL_NOT_RAL:
      p = _("Malformed bidirectional string");
      break;

    case STRINGPREP_BIDI_CONTAINS_PROHIBITED:
      p = _("Prohibited bidirectional code points in input");
      break;

    case STRINGPREP_TOO_SMALL_BUFFER:
      p = _("Output would exceed the buffer space provided");
      break;

    case STRINGPREP_PROFILE_ERROR:
      p = _("Error in stringprep profile definition");
      break;

    case STRINGPREP_FLAG_ERROR:
      p = _("Flag conflict with profile");
      break;

    case STRINGPREP_UNKNOWN_PROFILE:
     case STRINGPREP_UNKNOWN_PROFILE:
       p = _("Unknown profile");
       break;
    case STRINGPREP_ICONV_ERROR:
      p = _("Could not convert string in locale encoding.");
      break;
 
     case STRINGPREP_NFKC_FAILED:
       p = _("Unicode normalization failed (internal error)");
      break;

    default:
      p = _("Unknown error");
      break;
    }

  return p;
}
