int _yr_scan_verify_re_match(
    YR_SCAN_CONTEXT* context,
    YR_AC_MATCH* ac_match,
    uint8_t* data,
    size_t data_size,
    size_t data_base,
    size_t offset)
{
  CALLBACK_ARGS callback_args;
  RE_EXEC_FUNC exec;

  int forward_matches = -1;
  int backward_matches = -1;
  int flags = 0;

  if (STRING_IS_GREEDY_REGEXP(ac_match->string))
    flags |= RE_FLAGS_GREEDY;

  if (STRING_IS_NO_CASE(ac_match->string))
    flags |= RE_FLAGS_NO_CASE;

  if (STRING_IS_DOT_ALL(ac_match->string))
    flags |= RE_FLAGS_DOT_ALL;

  if (STRING_IS_FAST_REGEXP(ac_match->string))
    exec = yr_re_fast_exec;
  else
    exec = yr_re_exec;

  if (STRING_IS_ASCII(ac_match->string))
  {
    forward_matches = exec(
         ac_match->forward_code,
         data + offset,
         data_size - offset,
        offset,
        flags,
         NULL,
         NULL);
   }

  if (STRING_IS_WIDE(ac_match->string) && forward_matches == -1)
  {
    flags |= RE_FLAGS_WIDE;
    forward_matches = exec(
         ac_match->forward_code,
         data + offset,
         data_size - offset,
        offset,
        flags,
         NULL,
         NULL);
   }

  switch(forward_matches)
  {
    case -1:
      return ERROR_SUCCESS;
    case -2:
      return ERROR_INSUFFICIENT_MEMORY;
    case -3:
      return ERROR_TOO_MANY_MATCHES;
    case -4:
      return ERROR_TOO_MANY_RE_FIBERS;
    case -5:
      return ERROR_INTERNAL_FATAL_ERROR;
  }

  if (forward_matches == 0 && ac_match->backward_code == NULL)
    return ERROR_SUCCESS;

  callback_args.string = ac_match->string;
  callback_args.context = context;
  callback_args.data = data;
  callback_args.data_size = data_size;
  callback_args.data_base = data_base;
  callback_args.forward_matches = forward_matches;
  callback_args.full_word = STRING_IS_FULL_WORD(ac_match->string);

  if (ac_match->backward_code != NULL)
  {
     backward_matches = exec(
         ac_match->backward_code,
         data + offset,
        data_size - offset,
         offset,
         flags | RE_FLAGS_BACKWARDS | RE_FLAGS_EXHAUSTIVE,
         _yr_scan_match_callback,
        (void*) &callback_args);

    switch(backward_matches)
    {
      case -2:
        return ERROR_INSUFFICIENT_MEMORY;
      case -3:
        return ERROR_TOO_MANY_MATCHES;
      case -4:
        return ERROR_TOO_MANY_RE_FIBERS;
      case -5:
        return ERROR_INTERNAL_FATAL_ERROR;
    }
  }
  else
  {
    FAIL_ON_ERROR(_yr_scan_match_callback(
        data + offset, 0, flags, &callback_args));
  }

  return ERROR_SUCCESS;
}
