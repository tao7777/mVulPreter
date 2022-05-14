eval_condition(uschar *s, BOOL *resetok, BOOL *yield)
{
BOOL testfor = TRUE;
BOOL tempcond, combined_cond;
BOOL *subcondptr;
BOOL sub2_honour_dollar = TRUE;
int i, rc, cond_type, roffset;
int_eximarith_t num[2];
struct stat statbuf;
uschar name[256];
uschar *sub[10];

const pcre *re;
const uschar *rerror;

for (;;)
  {
  while (isspace(*s)) s++;
  if (*s == '!') { testfor = !testfor; s++; } else break;
  }

/* Numeric comparisons are symbolic */

if (*s == '=' || *s == '>' || *s == '<')
  {
  int p = 0;
  name[p++] = *s++;
  if (*s == '=')
    {
    name[p++] = '=';
    s++;
    }
  name[p] = 0;
  }

/* All other conditions are named */

else s = read_name(name, 256, s, US"_");

/* If we haven't read a name, it means some non-alpha character is first. */

if (name[0] == 0)
  {
  expand_string_message = string_sprintf("condition name expected, "
    "but found \"%.16s\"", s);
  return NULL;
  }

/* Find which condition we are dealing with, and switch on it */

cond_type = chop_match(name, cond_table, sizeof(cond_table)/sizeof(uschar *));
switch(cond_type)
  {
  /* def: tests for a non-empty variable, or for the existence of a header. If
  yield == NULL we are in a skipping state, and don't care about the answer. */

  case ECOND_DEF:
  if (*s != ':')
    {
    expand_string_message = US"\":\" expected after \"def\"";
    return NULL;
    }

  s = read_name(name, 256, s+1, US"_");

  /* Test for a header's existence. If the name contains a closing brace
  character, this may be a user error where the terminating colon has been
  omitted. Set a flag to adjust a subsequent error message in this case. */

  if (Ustrncmp(name, "h_", 2) == 0 ||
      Ustrncmp(name, "rh_", 3) == 0 ||
      Ustrncmp(name, "bh_", 3) == 0 ||
      Ustrncmp(name, "header_", 7) == 0 ||
      Ustrncmp(name, "rheader_", 8) == 0 ||
      Ustrncmp(name, "bheader_", 8) == 0)
    {
    s = read_header_name(name, 256, s);
    /* {-for-text-editors */
    if (Ustrchr(name, '}') != NULL) malformed_header = TRUE;
    if (yield != NULL) *yield =
      (find_header(name, TRUE, NULL, FALSE, NULL) != NULL) == testfor;
    }

  /* Test for a variable's having a non-empty value. A non-existent variable
  causes an expansion failure. */

  else
    {
    uschar *value = find_variable(name, TRUE, yield == NULL, NULL);
    if (value == NULL)
      {
      expand_string_message = (name[0] == 0)?
        string_sprintf("variable name omitted after \"def:\"") :
        string_sprintf("unknown variable \"%s\" after \"def:\"", name);
      check_variable_error_message(name);
      return NULL;
      }
    if (yield != NULL) *yield = (value[0] != 0) == testfor;
    }

  return s;


  /* first_delivery tests for first delivery attempt */

  case ECOND_FIRST_DELIVERY:
  if (yield != NULL) *yield = deliver_firsttime == testfor;
  return s;


  /* queue_running tests for any process started by a queue runner */

  case ECOND_QUEUE_RUNNING:
  if (yield != NULL) *yield = (queue_run_pid != (pid_t)0) == testfor;
  return s;


  /* exists:  tests for file existence
       isip:  tests for any IP address
      isip4:  tests for an IPv4 address
      isip6:  tests for an IPv6 address
        pam:  does PAM authentication
     radius:  does RADIUS authentication
   ldapauth:  does LDAP authentication
    pwcheck:  does Cyrus SASL pwcheck authentication
  */

  case ECOND_EXISTS:
  case ECOND_ISIP:
  case ECOND_ISIP4:
  case ECOND_ISIP6:
  case ECOND_PAM:
  case ECOND_RADIUS:
  case ECOND_LDAPAUTH:
  case ECOND_PWCHECK:

  while (isspace(*s)) s++;
  if (*s != '{') goto COND_FAILED_CURLY_START;		/* }-for-text-editors */

  sub[0] = expand_string_internal(s+1, TRUE, &s, yield == NULL, TRUE, resetok);
  if (sub[0] == NULL) return NULL;
  /* {-for-text-editors */
  if (*s++ != '}') goto COND_FAILED_CURLY_END;

  if (yield == NULL) return s;   /* No need to run the test if skipping */

  switch(cond_type)
    {
    case ECOND_EXISTS:
    if ((expand_forbid & RDO_EXISTS) != 0)
      {
      expand_string_message = US"File existence tests are not permitted";
      return NULL;
      }
    *yield = (Ustat(sub[0], &statbuf) == 0) == testfor;
    break;

    case ECOND_ISIP:
    case ECOND_ISIP4:
    case ECOND_ISIP6:
    rc = string_is_ip_address(sub[0], NULL);
    *yield = ((cond_type == ECOND_ISIP)? (rc != 0) :
             (cond_type == ECOND_ISIP4)? (rc == 4) : (rc == 6)) == testfor;
    break;

    /* Various authentication tests - all optionally compiled */

    case ECOND_PAM:
    #ifdef SUPPORT_PAM
    rc = auth_call_pam(sub[0], &expand_string_message);
    goto END_AUTH;
    #else
    goto COND_FAILED_NOT_COMPILED;
    #endif  /* SUPPORT_PAM */

    case ECOND_RADIUS:
    #ifdef RADIUS_CONFIG_FILE
    rc = auth_call_radius(sub[0], &expand_string_message);
    goto END_AUTH;
    #else
    goto COND_FAILED_NOT_COMPILED;
    #endif  /* RADIUS_CONFIG_FILE */

    case ECOND_LDAPAUTH:
    #ifdef LOOKUP_LDAP
      {
      /* Just to keep the interface the same */
      BOOL do_cache;
      int old_pool = store_pool;
      store_pool = POOL_SEARCH;
      rc = eldapauth_find((void *)(-1), NULL, sub[0], Ustrlen(sub[0]), NULL,
        &expand_string_message, &do_cache);
      store_pool = old_pool;
      }
    goto END_AUTH;
    #else
    goto COND_FAILED_NOT_COMPILED;
    #endif  /* LOOKUP_LDAP */

    case ECOND_PWCHECK:
    #ifdef CYRUS_PWCHECK_SOCKET
    rc = auth_call_pwcheck(sub[0], &expand_string_message);
    goto END_AUTH;
    #else
    goto COND_FAILED_NOT_COMPILED;
    #endif  /* CYRUS_PWCHECK_SOCKET */

    #if defined(SUPPORT_PAM) || defined(RADIUS_CONFIG_FILE) || \
        defined(LOOKUP_LDAP) || defined(CYRUS_PWCHECK_SOCKET)
    END_AUTH:
    if (rc == ERROR || rc == DEFER) return NULL;
    *yield = (rc == OK) == testfor;
    #endif
    }
  return s;


  /* call ACL (in a conditional context).  Accept true, deny false.
  Defer is a forced-fail.  Anything set by message= goes to $value.
  Up to ten parameters are used; we use the braces round the name+args
  like the saslauthd condition does, to permit a variable number of args.
  See also the expansion-item version EITEM_ACL and the traditional
  acl modifier ACLC_ACL.
  Since the ACL may allocate new global variables, tell our caller to not
  reclaim memory.
  */

  case ECOND_ACL:
    /* ${if acl {{name}{arg1}{arg2}...}  {yes}{no}} */
    {
    uschar *user_msg;
    BOOL cond = FALSE;
    int size = 0;
    int ptr = 0;

    while (isspace(*s)) s++;
    if (*s++ != '{') goto COND_FAILED_CURLY_START;	/*}*/

    switch(read_subs(sub, sizeof(sub)/sizeof(*sub), 1,
      &s, yield == NULL, TRUE, US"acl", resetok))
      {
      case 1: expand_string_message = US"too few arguments or bracketing "
        "error for acl";
      case 2:
      case 3: return NULL;
      }

    *resetok = FALSE;
    if (yield != NULL) switch(eval_acl(sub, sizeof(sub)/sizeof(*sub), &user_msg))
	{
	case OK:
	  cond = TRUE;
	case FAIL:
          lookup_value = NULL;
	  if (user_msg)
	    {
            lookup_value = string_cat(NULL, &size, &ptr, user_msg, Ustrlen(user_msg));
            lookup_value[ptr] = '\0';
	    }
	  *yield = cond == testfor;
	  break;

	case DEFER:
          expand_string_forcedfail = TRUE;
	default:
          expand_string_message = string_sprintf("error from acl \"%s\"", sub[0]);
	  return NULL;
	}
    return s;
    }


  /* saslauthd: does Cyrus saslauthd authentication. Four parameters are used:

     ${if saslauthd {{username}{password}{service}{realm}}  {yes}{no}}

  However, the last two are optional. That is why the whole set is enclosed
  in their own set of braces. */

  case ECOND_SASLAUTHD:
  #ifndef CYRUS_SASLAUTHD_SOCKET
  goto COND_FAILED_NOT_COMPILED;
  #else
  while (isspace(*s)) s++;
  if (*s++ != '{') goto COND_FAILED_CURLY_START;	/* }-for-text-editors */
  switch(read_subs(sub, 4, 2, &s, yield == NULL, TRUE, US"saslauthd", resetok))
    {
    case 1: expand_string_message = US"too few arguments or bracketing "
      "error for saslauthd";
    case 2:
    case 3: return NULL;
    }
  if (sub[2] == NULL) sub[3] = NULL;  /* realm if no service */
  if (yield != NULL)
    {
    int rc;
    rc = auth_call_saslauthd(sub[0], sub[1], sub[2], sub[3],
      &expand_string_message);
    if (rc == ERROR || rc == DEFER) return NULL;
    *yield = (rc == OK) == testfor;
    }
  return s;
  #endif /* CYRUS_SASLAUTHD_SOCKET */


  /* symbolic operators for numeric and string comparison, and a number of
  other operators, all requiring two arguments.

  crypteq:           encrypts plaintext and compares against an encrypted text,
                       using crypt(), crypt16(), MD5 or SHA-1
  inlist/inlisti:    checks if first argument is in the list of the second
  match:             does a regular expression match and sets up the numerical
                       variables if it succeeds
  match_address:     matches in an address list
  match_domain:      matches in a domain list
  match_ip:          matches a host list that is restricted to IP addresses
  match_local_part:  matches in a local part list
  */

  case ECOND_MATCH_ADDRESS:
  case ECOND_MATCH_DOMAIN:
  case ECOND_MATCH_IP:
  case ECOND_MATCH_LOCAL_PART:
#ifndef EXPAND_LISTMATCH_RHS
    sub2_honour_dollar = FALSE;
#endif
    /* FALLTHROUGH */

  case ECOND_CRYPTEQ:
  case ECOND_INLIST:
  case ECOND_INLISTI:
  case ECOND_MATCH:

  case ECOND_NUM_L:     /* Numerical comparisons */
  case ECOND_NUM_LE:
  case ECOND_NUM_E:
  case ECOND_NUM_EE:
  case ECOND_NUM_G:
  case ECOND_NUM_GE:

  case ECOND_STR_LT:    /* String comparisons */
  case ECOND_STR_LTI:
  case ECOND_STR_LE:
  case ECOND_STR_LEI:
  case ECOND_STR_EQ:
  case ECOND_STR_EQI:
  case ECOND_STR_GT:
  case ECOND_STR_GTI:
  case ECOND_STR_GE:
  case ECOND_STR_GEI:

  for (i = 0; i < 2; i++)
    {
    /* Sometimes, we don't expand substrings; too many insecure configurations
    created using match_address{}{} and friends, where the second param
    includes information from untrustworthy sources. */
    BOOL honour_dollar = TRUE;
    if ((i > 0) && !sub2_honour_dollar)
      honour_dollar = FALSE;

    while (isspace(*s)) s++;
    if (*s != '{')
      {
      if (i == 0) goto COND_FAILED_CURLY_START;
      expand_string_message = string_sprintf("missing 2nd string in {} "
        "after \"%s\"", name);
      return NULL;
      }
    sub[i] = expand_string_internal(s+1, TRUE, &s, yield == NULL,
        honour_dollar, resetok);
    if (sub[i] == NULL) return NULL;
    if (*s++ != '}') goto COND_FAILED_CURLY_END;

    /* Convert to numerical if required; we know that the names of all the
    conditions that compare numbers do not start with a letter. This just saves
    checking for them individually. */

    if (!isalpha(name[0]) && yield != NULL)
      {
      if (sub[i][0] == 0)
        {
        num[i] = 0;
        DEBUG(D_expand)
          debug_printf("empty string cast to zero for numerical comparison\n");
         }
       else
         {
        num[i] = expanded_string_integer(sub[i], FALSE);
         if (expand_string_message != NULL) return NULL;
         }
       }
    }

  /* Result not required */

  if (yield == NULL) return s;

  /* Do an appropriate comparison */

  switch(cond_type)
    {
    case ECOND_NUM_E:
    case ECOND_NUM_EE:
    tempcond = (num[0] == num[1]);
    break;

    case ECOND_NUM_G:
    tempcond = (num[0] > num[1]);
    break;

    case ECOND_NUM_GE:
    tempcond = (num[0] >= num[1]);
    break;

    case ECOND_NUM_L:
    tempcond = (num[0] < num[1]);
    break;

    case ECOND_NUM_LE:
    tempcond = (num[0] <= num[1]);
    break;

    case ECOND_STR_LT:
    tempcond = (Ustrcmp(sub[0], sub[1]) < 0);
    break;

    case ECOND_STR_LTI:
    tempcond = (strcmpic(sub[0], sub[1]) < 0);
    break;

    case ECOND_STR_LE:
    tempcond = (Ustrcmp(sub[0], sub[1]) <= 0);
    break;

    case ECOND_STR_LEI:
    tempcond = (strcmpic(sub[0], sub[1]) <= 0);
    break;

    case ECOND_STR_EQ:
    tempcond = (Ustrcmp(sub[0], sub[1]) == 0);
    break;

    case ECOND_STR_EQI:
    tempcond = (strcmpic(sub[0], sub[1]) == 0);
    break;

    case ECOND_STR_GT:
    tempcond = (Ustrcmp(sub[0], sub[1]) > 0);
    break;

    case ECOND_STR_GTI:
    tempcond = (strcmpic(sub[0], sub[1]) > 0);
    break;

    case ECOND_STR_GE:
    tempcond = (Ustrcmp(sub[0], sub[1]) >= 0);
    break;

    case ECOND_STR_GEI:
    tempcond = (strcmpic(sub[0], sub[1]) >= 0);
    break;

    case ECOND_MATCH:   /* Regular expression match */
    re = pcre_compile(CS sub[1], PCRE_COPT, (const char **)&rerror, &roffset,
      NULL);
    if (re == NULL)
      {
      expand_string_message = string_sprintf("regular expression error in "
        "\"%s\": %s at offset %d", sub[1], rerror, roffset);
      return NULL;
      }
    tempcond = regex_match_and_setup(re, sub[0], 0, -1);
    break;

    case ECOND_MATCH_ADDRESS:  /* Match in an address list */
    rc = match_address_list(sub[0], TRUE, FALSE, &(sub[1]), NULL, -1, 0, NULL);
    goto MATCHED_SOMETHING;

    case ECOND_MATCH_DOMAIN:   /* Match in a domain list */
    rc = match_isinlist(sub[0], &(sub[1]), 0, &domainlist_anchor, NULL,
      MCL_DOMAIN + MCL_NOEXPAND, TRUE, NULL);
    goto MATCHED_SOMETHING;

    case ECOND_MATCH_IP:       /* Match IP address in a host list */
    if (sub[0][0] != 0 && string_is_ip_address(sub[0], NULL) == 0)
      {
      expand_string_message = string_sprintf("\"%s\" is not an IP address",
        sub[0]);
      return NULL;
      }
    else
      {
      unsigned int *nullcache = NULL;
      check_host_block cb;

      cb.host_name = US"";
      cb.host_address = sub[0];

      /* If the host address starts off ::ffff: it is an IPv6 address in
      IPv4-compatible mode. Find the IPv4 part for checking against IPv4
      addresses. */

      cb.host_ipv4 = (Ustrncmp(cb.host_address, "::ffff:", 7) == 0)?
        cb.host_address + 7 : cb.host_address;

      rc = match_check_list(
             &sub[1],                   /* the list */
             0,                         /* separator character */
             &hostlist_anchor,          /* anchor pointer */
             &nullcache,                /* cache pointer */
             check_host,                /* function for testing */
             &cb,                       /* argument for function */
             MCL_HOST,                  /* type of check */
             sub[0],                    /* text for debugging */
             NULL);                     /* where to pass back data */
      }
    goto MATCHED_SOMETHING;

    case ECOND_MATCH_LOCAL_PART:
    rc = match_isinlist(sub[0], &(sub[1]), 0, &localpartlist_anchor, NULL,
      MCL_LOCALPART + MCL_NOEXPAND, TRUE, NULL);
    /* Fall through */
    /* VVVVVVVVVVVV */
    MATCHED_SOMETHING:
    switch(rc)
      {
      case OK:
      tempcond = TRUE;
      break;

      case FAIL:
      tempcond = FALSE;
      break;

      case DEFER:
      expand_string_message = string_sprintf("unable to complete match "
        "against \"%s\": %s", sub[1], search_error_message);
      return NULL;
      }

    break;

    /* Various "encrypted" comparisons. If the second string starts with
    "{" then an encryption type is given. Default to crypt() or crypt16()
    (build-time choice). */
    /* }-for-text-editors */

    case ECOND_CRYPTEQ:
    #ifndef SUPPORT_CRYPTEQ
    goto COND_FAILED_NOT_COMPILED;
    #else
    if (strncmpic(sub[1], US"{md5}", 5) == 0)
      {
      int sublen = Ustrlen(sub[1]+5);
      md5 base;
      uschar digest[16];

      md5_start(&base);
      md5_end(&base, (uschar *)sub[0], Ustrlen(sub[0]), digest);

      /* If the length that we are comparing against is 24, the MD5 digest
      is expressed as a base64 string. This is the way LDAP does it. However,
      some other software uses a straightforward hex representation. We assume
      this if the length is 32. Other lengths fail. */

      if (sublen == 24)
        {
        uschar *coded = auth_b64encode((uschar *)digest, 16);
        DEBUG(D_auth) debug_printf("crypteq: using MD5+B64 hashing\n"
          "  subject=%s\n  crypted=%s\n", coded, sub[1]+5);
        tempcond = (Ustrcmp(coded, sub[1]+5) == 0);
        }
      else if (sublen == 32)
        {
        int i;
        uschar coded[36];
        for (i = 0; i < 16; i++) sprintf(CS (coded+2*i), "%02X", digest[i]);
        coded[32] = 0;
        DEBUG(D_auth) debug_printf("crypteq: using MD5+hex hashing\n"
          "  subject=%s\n  crypted=%s\n", coded, sub[1]+5);
        tempcond = (strcmpic(coded, sub[1]+5) == 0);
        }
      else
        {
        DEBUG(D_auth) debug_printf("crypteq: length for MD5 not 24 or 32: "
          "fail\n  crypted=%s\n", sub[1]+5);
        tempcond = FALSE;
        }
      }

    else if (strncmpic(sub[1], US"{sha1}", 6) == 0)
      {
      int sublen = Ustrlen(sub[1]+6);
      sha1 base;
      uschar digest[20];

      sha1_start(&base);
      sha1_end(&base, (uschar *)sub[0], Ustrlen(sub[0]), digest);

      /* If the length that we are comparing against is 28, assume the SHA1
      digest is expressed as a base64 string. If the length is 40, assume a
      straightforward hex representation. Other lengths fail. */

      if (sublen == 28)
        {
        uschar *coded = auth_b64encode((uschar *)digest, 20);
        DEBUG(D_auth) debug_printf("crypteq: using SHA1+B64 hashing\n"
          "  subject=%s\n  crypted=%s\n", coded, sub[1]+6);
        tempcond = (Ustrcmp(coded, sub[1]+6) == 0);
        }
      else if (sublen == 40)
        {
        int i;
        uschar coded[44];
        for (i = 0; i < 20; i++) sprintf(CS (coded+2*i), "%02X", digest[i]);
        coded[40] = 0;
        DEBUG(D_auth) debug_printf("crypteq: using SHA1+hex hashing\n"
          "  subject=%s\n  crypted=%s\n", coded, sub[1]+6);
        tempcond = (strcmpic(coded, sub[1]+6) == 0);
        }
      else
        {
        DEBUG(D_auth) debug_printf("crypteq: length for SHA-1 not 28 or 40: "
          "fail\n  crypted=%s\n", sub[1]+6);
	tempcond = FALSE;
        }
      }

    else   /* {crypt} or {crypt16} and non-{ at start */
           /* }-for-text-editors */
      {
      int which = 0;
      uschar *coded;

      if (strncmpic(sub[1], US"{crypt}", 7) == 0)
        {
        sub[1] += 7;
        which = 1;
        }
      else if (strncmpic(sub[1], US"{crypt16}", 9) == 0)
        {
        sub[1] += 9;
        which = 2;
        }
      else if (sub[1][0] == '{')		/* }-for-text-editors */
        {
        expand_string_message = string_sprintf("unknown encryption mechanism "
          "in \"%s\"", sub[1]);
        return NULL;
        }

      switch(which)
        {
        case 0:  coded = US DEFAULT_CRYPT(CS sub[0], CS sub[1]); break;
        case 1:  coded = US crypt(CS sub[0], CS sub[1]); break;
        default: coded = US crypt16(CS sub[0], CS sub[1]); break;
        }

      #define STR(s) # s
      #define XSTR(s) STR(s)
      DEBUG(D_auth) debug_printf("crypteq: using %s()\n"
        "  subject=%s\n  crypted=%s\n",
        (which == 0)? XSTR(DEFAULT_CRYPT) : (which == 1)? "crypt" : "crypt16",
        coded, sub[1]);
      #undef STR
      #undef XSTR

      /* If the encrypted string contains fewer than two characters (for the
      salt), force failure. Otherwise we get false positives: with an empty
      string the yield of crypt() is an empty string! */

      tempcond = (Ustrlen(sub[1]) < 2)? FALSE :
        (Ustrcmp(coded, sub[1]) == 0);
      }
    break;
    #endif  /* SUPPORT_CRYPTEQ */

    case ECOND_INLIST:
    case ECOND_INLISTI:
      {
      int sep = 0;
      uschar *save_iterate_item = iterate_item;
      int (*compare)(const uschar *, const uschar *);

      tempcond = FALSE;
      if (cond_type == ECOND_INLISTI)
        compare = strcmpic;
      else
        compare = (int (*)(const uschar *, const uschar *)) strcmp;

      while ((iterate_item = string_nextinlist(&sub[1], &sep, NULL, 0)) != NULL)
        if (compare(sub[0], iterate_item) == 0)
          {
          tempcond = TRUE;
          break;
          }
      iterate_item = save_iterate_item;
      }

    }   /* Switch for comparison conditions */

  *yield = tempcond == testfor;
  return s;    /* End of comparison conditions */


  /* and/or: computes logical and/or of several conditions */

  case ECOND_AND:
  case ECOND_OR:
  subcondptr = (yield == NULL)? NULL : &tempcond;
  combined_cond = (cond_type == ECOND_AND);

  while (isspace(*s)) s++;
  if (*s++ != '{') goto COND_FAILED_CURLY_START;	/* }-for-text-editors */

  for (;;)
    {
    while (isspace(*s)) s++;
    /* {-for-text-editors */
    if (*s == '}') break;
    if (*s != '{')					/* }-for-text-editors */
      {
      expand_string_message = string_sprintf("each subcondition "
        "inside an \"%s{...}\" condition must be in its own {}", name);
      return NULL;
      }

    if (!(s = eval_condition(s+1, resetok, subcondptr)))
      {
      expand_string_message = string_sprintf("%s inside \"%s{...}\" condition",
        expand_string_message, name);
      return NULL;
      }
    while (isspace(*s)) s++;

    /* {-for-text-editors */
    if (*s++ != '}')
      {
      /* {-for-text-editors */
      expand_string_message = string_sprintf("missing } at end of condition "
        "inside \"%s\" group", name);
      return NULL;
      }

    if (yield != NULL)
      {
      if (cond_type == ECOND_AND)
        {
        combined_cond &= tempcond;
        if (!combined_cond) subcondptr = NULL;  /* once false, don't */
        }                                       /* evaluate any more */
      else
        {
        combined_cond |= tempcond;
        if (combined_cond) subcondptr = NULL;   /* once true, don't */
        }                                       /* evaluate any more */
      }
    }

  if (yield != NULL) *yield = (combined_cond == testfor);
  return ++s;


  /* forall/forany: iterates a condition with different values */

  case ECOND_FORALL:
  case ECOND_FORANY:
    {
    int sep = 0;
    uschar *save_iterate_item = iterate_item;

    while (isspace(*s)) s++;
    if (*s++ != '{') goto COND_FAILED_CURLY_START;	/* }-for-text-editors */
    sub[0] = expand_string_internal(s, TRUE, &s, (yield == NULL), TRUE, resetok);
    if (sub[0] == NULL) return NULL;
    /* {-for-text-editors */
    if (*s++ != '}') goto COND_FAILED_CURLY_END;

    while (isspace(*s)) s++;
    if (*s++ != '{') goto COND_FAILED_CURLY_START;	/* }-for-text-editors */

    sub[1] = s;

    /* Call eval_condition once, with result discarded (as if scanning a
    "false" part). This allows us to find the end of the condition, because if
    the list it empty, we won't actually evaluate the condition for real. */

    if (!(s = eval_condition(sub[1], resetok, NULL)))
      {
      expand_string_message = string_sprintf("%s inside \"%s\" condition",
        expand_string_message, name);
      return NULL;
      }
    while (isspace(*s)) s++;

    /* {-for-text-editors */
    if (*s++ != '}')
      {
      /* {-for-text-editors */
      expand_string_message = string_sprintf("missing } at end of condition "
        "inside \"%s\"", name);
      return NULL;
      }

    if (yield != NULL) *yield = !testfor;
    while ((iterate_item = string_nextinlist(&sub[0], &sep, NULL, 0)) != NULL)
      {
      DEBUG(D_expand) debug_printf("%s: $item = \"%s\"\n", name, iterate_item);
      if (!eval_condition(sub[1], resetok, &tempcond))
        {
        expand_string_message = string_sprintf("%s inside \"%s\" condition",
          expand_string_message, name);
        iterate_item = save_iterate_item;
        return NULL;
        }
      DEBUG(D_expand) debug_printf("%s: condition evaluated to %s\n", name,
        tempcond? "true":"false");

      if (yield != NULL) *yield = (tempcond == testfor);
      if (tempcond == (cond_type == ECOND_FORANY)) break;
      }

    iterate_item = save_iterate_item;
    return s;
    }


  /* The bool{} expansion condition maps a string to boolean.
  The values supported should match those supported by the ACL condition
  (acl.c, ACLC_CONDITION) so that we keep to a minimum the different ideas
  of true/false.  Note that Router "condition" rules have a different
  interpretation, where general data can be used and only a few values
  map to FALSE.
  Note that readconf.c boolean matching, for boolean configuration options,
  only matches true/yes/false/no.
  The bool_lax{} condition matches the Router logic, which is much more
  liberal. */
  case ECOND_BOOL:
  case ECOND_BOOL_LAX:
    {
    uschar *sub_arg[1];
    uschar *t, *t2;
    uschar *ourname;
    size_t len;
    BOOL boolvalue = FALSE;
    while (isspace(*s)) s++;
    if (*s != '{') goto COND_FAILED_CURLY_START;	/* }-for-text-editors */
    ourname = cond_type == ECOND_BOOL_LAX ? US"bool_lax" : US"bool";
    switch(read_subs(sub_arg, 1, 1, &s, yield == NULL, FALSE, ourname, resetok))
      {
      case 1: expand_string_message = string_sprintf(
                  "too few arguments or bracketing error for %s",
                  ourname);
      /*FALLTHROUGH*/
      case 2:
      case 3: return NULL;
      }
    t = sub_arg[0];
    while (isspace(*t)) t++;
    len = Ustrlen(t);
    if (len)
      {
      /* trailing whitespace: seems like a good idea to ignore it too */
      t2 = t + len - 1;
      while (isspace(*t2)) t2--;
      if (t2 != (t + len))
        {
        *++t2 = '\0';
        len = t2 - t;
        }
      }
    DEBUG(D_expand)
      debug_printf("considering %s: %s\n", ourname, len ? t : US"<empty>");
    /* logic for the lax case from expand_check_condition(), which also does
    expands, and the logic is both short and stable enough that there should
    be no maintenance burden from replicating it. */
    if (len == 0)
      boolvalue = FALSE;
    else if (*t == '-'
	     ? Ustrspn(t+1, "0123456789") == len-1
	     : Ustrspn(t,   "0123456789") == len)
      {
      boolvalue = (Uatoi(t) == 0) ? FALSE : TRUE;
      /* expand_check_condition only does a literal string "0" check */
      if ((cond_type == ECOND_BOOL_LAX) && (len > 1))
        boolvalue = TRUE;
      }
    else if (strcmpic(t, US"true") == 0 || strcmpic(t, US"yes") == 0)
      boolvalue = TRUE;
    else if (strcmpic(t, US"false") == 0 || strcmpic(t, US"no") == 0)
      boolvalue = FALSE;
    else if (cond_type == ECOND_BOOL_LAX)
      boolvalue = TRUE;
    else
      {
      expand_string_message = string_sprintf("unrecognised boolean "
       "value \"%s\"", t);
      return NULL;
      }
    if (yield != NULL) *yield = (boolvalue == testfor);
    return s;
    }

  /* Unknown condition */

  default:
  expand_string_message = string_sprintf("unknown condition \"%s\"", name);
  return NULL;
  }   /* End switch on condition type */

/* Missing braces at start and end of data */

COND_FAILED_CURLY_START:
expand_string_message = string_sprintf("missing { after \"%s\"", name);
return NULL;

COND_FAILED_CURLY_END:
expand_string_message = string_sprintf("missing } at end of \"%s\" condition",
  name);
return NULL;

/* A condition requires code that is not compiled */

#if !defined(SUPPORT_PAM) || !defined(RADIUS_CONFIG_FILE) || \
    !defined(LOOKUP_LDAP) || !defined(CYRUS_PWCHECK_SOCKET) || \
    !defined(SUPPORT_CRYPTEQ) || !defined(CYRUS_SASLAUTHD_SOCKET)
COND_FAILED_NOT_COMPILED:
expand_string_message = string_sprintf("support for \"%s\" not compiled",
  name);
return NULL;
#endif
}
