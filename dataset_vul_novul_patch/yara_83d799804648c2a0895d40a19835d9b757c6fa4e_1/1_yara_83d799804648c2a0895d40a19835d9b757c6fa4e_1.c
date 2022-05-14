int yr_re_match(
    RE* re,
    const char* target)
{
  return yr_re_exec(
       re->code,
       (uint8_t*) target,
       strlen(target),
       re->flags | RE_FLAGS_SCAN,
       NULL,
       NULL);
}
