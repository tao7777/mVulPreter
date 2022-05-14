 int yr_re_exec(
     uint8_t* re_code,
     uint8_t* input_data,
    size_t input_forwards_size,
    size_t input_backwards_size,
     int flags,
     RE_MATCH_CALLBACK_FUNC callback,
     void* callback_args)
{
  uint8_t* ip;
  uint8_t* input;
  uint8_t mask;
  uint8_t value;

  RE_FIBER_LIST fibers;
  RE_THREAD_STORAGE* storage;
  RE_FIBER* fiber;
  RE_FIBER* next_fiber;

  int error;
  int bytes_matched;
  int max_bytes_matched;
  int match;
  int character_size;
  int input_incr;
  int kill;
  int action;
  int result = -1;

  #define ACTION_NONE       0
  #define ACTION_CONTINUE   1
   #define ACTION_KILL       2
   #define ACTION_KILL_TAIL  3
 
  #define prolog { \
      if ((bytes_matched >= max_bytes_matched) || \
          (character_size == 2 && *(input + 1) != 0)) \
       { \
         action = ACTION_KILL; \
         break; \
      } \
    }
 
  #define fail_if_error(e) { \
      switch (e) { \
         case ERROR_INSUFFICIENT_MEMORY: \
           return -2; \
         case ERROR_TOO_MANY_RE_FIBERS: \
           return -4; \
      } \
    }
 
   if (_yr_re_alloc_storage(&storage) != ERROR_SUCCESS)
     return -2;

  if (flags & RE_FLAGS_WIDE)
    character_size = 2;
  else
    character_size = 1;

  input = input_data;
  input_incr = character_size;
 
   if (flags & RE_FLAGS_BACKWARDS)
   {
    max_bytes_matched = (int) yr_min(input_backwards_size, RE_SCAN_LIMIT);
     input -= character_size;
     input_incr = -input_incr;
   }
  else
  {
    max_bytes_matched = (int) yr_min(input_forwards_size, RE_SCAN_LIMIT);
  }
 
  // character_size is 2 and max_bytes_matched is odd we are ignoring the
 
   max_bytes_matched = max_bytes_matched - max_bytes_matched % character_size;
  bytes_matched = 0;

  error = _yr_re_fiber_create(&storage->fiber_pool, &fiber);
  fail_if_error(error);

  fiber->ip = re_code;
  fibers.head = fiber;
  fibers.tail = fiber;

  error = _yr_re_fiber_sync(&fibers, &storage->fiber_pool, fiber);
  fail_if_error(error);

  while (fibers.head != NULL)
  {
    fiber = fibers.head;

    while(fiber != NULL)
    {
      ip = fiber->ip;
      action = ACTION_NONE;

      switch(*ip)
      {
        case RE_OPCODE_ANY:
          prolog;
          match = (flags & RE_FLAGS_DOT_ALL) || (*input != 0x0A);
          action = match ? ACTION_NONE : ACTION_KILL;
          fiber->ip += 1;
          break;

        case RE_OPCODE_REPEAT_ANY_GREEDY:
        case RE_OPCODE_REPEAT_ANY_UNGREEDY:
          prolog;
          match = (flags & RE_FLAGS_DOT_ALL) || (*input != 0x0A);
          action = match ? ACTION_NONE : ACTION_KILL;


          break;

        case RE_OPCODE_LITERAL:
          prolog;
          if (flags & RE_FLAGS_NO_CASE)
            match = yr_lowercase[*input] == yr_lowercase[*(ip + 1)];
          else
            match = (*input == *(ip + 1));
          action = match ? ACTION_NONE : ACTION_KILL;
          fiber->ip += 2;
          break;

        case RE_OPCODE_MASKED_LITERAL:
          prolog;
          value = *(int16_t*)(ip + 1) & 0xFF;
          mask = *(int16_t*)(ip + 1) >> 8;


          match = ((*input & mask) == value);
          action = match ? ACTION_NONE : ACTION_KILL;
          fiber->ip += 3;
          break;

        case RE_OPCODE_CLASS:
          prolog;
          match = CHAR_IN_CLASS(*input, ip + 1);
          if (!match && (flags & RE_FLAGS_NO_CASE))
            match = CHAR_IN_CLASS(yr_altercase[*input], ip + 1);
          action = match ? ACTION_NONE : ACTION_KILL;
          fiber->ip += 33;
          break;
 
         case RE_OPCODE_WORD_CHAR:
           prolog;
          match = _yr_re_is_word_char(input, character_size);
           action = match ? ACTION_NONE : ACTION_KILL;
           fiber->ip += 1;
           break;
 
         case RE_OPCODE_NON_WORD_CHAR:
           prolog;
          match = !_yr_re_is_word_char(input, character_size);
           action = match ? ACTION_NONE : ACTION_KILL;
           fiber->ip += 1;
           break;

        case RE_OPCODE_SPACE:
        case RE_OPCODE_NON_SPACE:

          prolog;

          switch(*input)
          {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            case '\v':
            case '\f':
              match = TRUE;
              break;
            default:
              match = FALSE;
          }

          if (*ip == RE_OPCODE_NON_SPACE)
            match = !match;

          action = match ? ACTION_NONE : ACTION_KILL;
          fiber->ip += 1;
          break;

        case RE_OPCODE_DIGIT:
          prolog;
          match = isdigit(*input);
          action = match ? ACTION_NONE : ACTION_KILL;
          fiber->ip += 1;
          break;

        case RE_OPCODE_NON_DIGIT:
          prolog;
          match = !isdigit(*input);
          action = match ? ACTION_NONE : ACTION_KILL;
          fiber->ip += 1;
          break;

         case RE_OPCODE_WORD_BOUNDARY:
         case RE_OPCODE_NON_WORD_BOUNDARY:
 
          if (bytes_matched == 0 && input_backwards_size < character_size)
          {
             match = TRUE;
          }
           else if (bytes_matched >= max_bytes_matched)
          {
             match = TRUE;
          }
           else
          {
            assert(input <  input_data + input_forwards_size);
            assert(input >= input_data - input_backwards_size);

            assert(input - input_incr <  input_data + input_forwards_size);
            assert(input - input_incr >= input_data - input_backwards_size);

            match = _yr_re_is_word_char(input, character_size) != \
                    _yr_re_is_word_char(input - input_incr, character_size);
          }
 
           if (*ip == RE_OPCODE_NON_WORD_BOUNDARY)
             match = !match;

          action = match ? ACTION_CONTINUE : ACTION_KILL;
          fiber->ip += 1;
          break;
 
         case RE_OPCODE_MATCH_AT_START:
           if (flags & RE_FLAGS_BACKWARDS)
            kill = input_backwards_size > (size_t) bytes_matched;
           else
            kill = input_backwards_size > 0 || (bytes_matched != 0);
           action = kill ? ACTION_KILL : ACTION_CONTINUE;
           fiber->ip += 1;
           break;
 
         case RE_OPCODE_MATCH_AT_END:
           kill = flags & RE_FLAGS_BACKWARDS ||
                 input_forwards_size > (size_t) bytes_matched;
           action = kill ? ACTION_KILL : ACTION_CONTINUE;
           fiber->ip += 1;
           break;

        case RE_OPCODE_MATCH:

          result = bytes_matched;

          if (flags & RE_FLAGS_EXHAUSTIVE)
          {
            if (callback != NULL)
            {
              int cb_result;

              if (flags & RE_FLAGS_BACKWARDS)
                cb_result = callback(
                    input + character_size,
                    bytes_matched,
                    flags,
                    callback_args);
              else
                cb_result = callback(
                    input_data,
                    bytes_matched,
                    flags,
                    callback_args);

              switch(cb_result)
              {
                case ERROR_INSUFFICIENT_MEMORY:
                  return -2;
                case ERROR_TOO_MANY_MATCHES:
                  return -3;
                default:
                  if (cb_result != ERROR_SUCCESS)
                    return -4;
              }
            }

            action = ACTION_KILL;
          }
          else
          {
            action = ACTION_KILL_TAIL;
          }

          break;

        default:
          assert(FALSE);
      }

      switch(action)
      {
        case ACTION_KILL:
          fiber = _yr_re_fiber_kill(&fibers, &storage->fiber_pool, fiber);
          break;

        case ACTION_KILL_TAIL:
          _yr_re_fiber_kill_tail(&fibers, &storage->fiber_pool, fiber);
          fiber = NULL;
          break;

        case ACTION_CONTINUE:
          error = _yr_re_fiber_sync(&fibers, &storage->fiber_pool, fiber);
          fail_if_error(error);
          break;

        default:
          next_fiber = fiber->next;
          error = _yr_re_fiber_sync(&fibers, &storage->fiber_pool, fiber);
          fail_if_error(error);
          fiber = next_fiber;
       }
     }
 
     input += input_incr;
     bytes_matched += character_size;
 
    if (flags & RE_FLAGS_SCAN && bytes_matched < max_bytes_matched)
    {
      error = _yr_re_fiber_create(&storage->fiber_pool, &fiber);
      fail_if_error(error);

      fiber->ip = re_code;
      _yr_re_fiber_append(&fibers, fiber);

      error = _yr_re_fiber_sync(&fibers, &storage->fiber_pool, fiber);
      fail_if_error(error);
    }
  }

  return result;
}
