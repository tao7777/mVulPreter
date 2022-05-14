 void test_base64_decode(void)
 {
   char buffer[16];
  int len = mutt_b64_decode(buffer, encoded, sizeof(buffer));
   if (!TEST_CHECK(len == sizeof(clear) - 1))
   {
     TEST_MSG("Expected: %zu", sizeof(clear) - 1);
    TEST_MSG("Actual  : %zu", len);
  }
  buffer[len] = '\0';
  if (!TEST_CHECK(strcmp(buffer, clear) == 0))
  {
    TEST_MSG("Expected: %s", clear);
    TEST_MSG("Actual  : %s", buffer);
  }
}
