int _yr_scan_match_callback(
    uint8_t* match_data,
    int32_t match_length,
    int flags,
    void* args)
{
  CALLBACK_ARGS* callback_args = (CALLBACK_ARGS*) args;

  YR_STRING* string = callback_args->string;
  YR_MATCH* new_match;

  int result = ERROR_SUCCESS;
  int tidx = callback_args->context->tidx;

  size_t match_offset = match_data - callback_args->data;

   match_length += callback_args->forward_matches;
 
  // make sure that match fits into the data.
  assert(match_offset + match_length <= callback_args->data_size);

   if (callback_args->full_word)
   {
     if (flags & RE_FLAGS_WIDE)
    {
      if (match_offset >= 2 &&
          *(match_data - 1) == 0 &&
          isalnum(*(match_data - 2)))
        return ERROR_SUCCESS;

      if (match_offset + match_length + 1 < callback_args->data_size &&
          *(match_data + match_length + 1) == 0 &&
          isalnum(*(match_data + match_length)))
        return ERROR_SUCCESS;
    }
    else
    {
      if (match_offset >= 1 &&
          isalnum(*(match_data - 1)))
        return ERROR_SUCCESS;

      if (match_offset + match_length < callback_args->data_size &&
          isalnum(*(match_data + match_length)))
        return ERROR_SUCCESS;
    }
  }

  if (STRING_IS_CHAIN_PART(string))
  {
    result = _yr_scan_verify_chained_string_match(
        string,
        callback_args->context,
        match_data,
        callback_args->data_base,
        match_offset,
        match_length);
  }
  else
  {
    if (string->matches[tidx].count == 0)
    {

      FAIL_ON_ERROR(yr_arena_write_data(
          callback_args->context->matching_strings_arena,
          &string,
          sizeof(string),
          NULL));
    }

    FAIL_ON_ERROR(yr_arena_allocate_memory(
        callback_args->context->matches_arena,
        sizeof(YR_MATCH),
        (void**) &new_match));

    new_match->data_length = yr_min(match_length, MAX_MATCH_DATA);

    FAIL_ON_ERROR(yr_arena_write_data(
        callback_args->context->matches_arena,
        match_data,
        new_match->data_length,
        (void**) &new_match->data));

    if (result == ERROR_SUCCESS)
    {
      new_match->base = callback_args->data_base;
      new_match->offset = match_offset;
      new_match->match_length = match_length;
      new_match->prev = NULL;
      new_match->next = NULL;

      FAIL_ON_ERROR(_yr_scan_add_match_to_list(
          new_match,
          &string->matches[tidx],
          STRING_IS_GREEDY_REGEXP(string)));
    }
  }

  return result;
}
