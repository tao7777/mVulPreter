test_bson_validate (void)
{
   char filename[64];
   size_t offset;
   bson_t *b;
   int i;
   bson_error_t error;

   for (i = 1; i <= 38; i++) {
      bson_snprintf (filename, sizeof filename, "test%u.bson", i);
      b = get_bson (filename);
      BSON_ASSERT (bson_validate (b, BSON_VALIDATE_NONE, &offset));
      bson_destroy (b);
   }

   b = get_bson ("codewscope.bson");
   BSON_ASSERT (bson_validate (b, BSON_VALIDATE_NONE, &offset));
   bson_destroy (b);

   b = get_bson ("empty_key.bson");
   BSON_ASSERT (bson_validate (b,
                               BSON_VALIDATE_NONE | BSON_VALIDATE_UTF8 |
                                  BSON_VALIDATE_DOLLAR_KEYS |
                                  BSON_VALIDATE_DOT_KEYS,
                               &offset));
   bson_destroy (b);

#define VALIDATE_TEST(_filename, _flags, _offset, _flag, _msg)     \
   b = get_bson (_filename);                                       \
   BSON_ASSERT (!bson_validate (b, _flags, &offset));              \
   ASSERT_CMPSIZE_T (offset, ==, (size_t) _offset);                \
   BSON_ASSERT (!bson_validate_with_error (b, _flags, &error));    \
   ASSERT_ERROR_CONTAINS (error, BSON_ERROR_INVALID, _flag, _msg); \
   bson_destroy (b)

   VALIDATE_TEST ("overflow2.bson",
                  BSON_VALIDATE_NONE,
                  9,
                  BSON_VALIDATE_NONE,
                  "corrupt BSON");
   VALIDATE_TEST ("trailingnull.bson",
                  BSON_VALIDATE_NONE,
                  14,
                  BSON_VALIDATE_NONE,
                  "corrupt BSON");
   VALIDATE_TEST ("dollarquery.bson",
                  BSON_VALIDATE_DOLLAR_KEYS | BSON_VALIDATE_DOT_KEYS,
                  4,
                  BSON_VALIDATE_DOLLAR_KEYS,
                  "keys cannot begin with \"$\": \"$query\"");
   VALIDATE_TEST ("dotquery.bson",
                  BSON_VALIDATE_DOLLAR_KEYS | BSON_VALIDATE_DOT_KEYS,
                  4,
                  BSON_VALIDATE_DOT_KEYS,
                  "keys cannot contain \".\": \"abc.def\"");
   VALIDATE_TEST ("overflow3.bson",
                  BSON_VALIDATE_NONE,
                  9,
                  BSON_VALIDATE_NONE,
                  "corrupt BSON");
   /* same outcome as above, despite different flags */
   VALIDATE_TEST ("overflow3.bson",
                  BSON_VALIDATE_UTF8,
                  9,
                  BSON_VALIDATE_NONE,
                  "corrupt BSON");
   VALIDATE_TEST ("overflow4.bson",
                  BSON_VALIDATE_NONE,
                  9,
                  BSON_VALIDATE_NONE,
                  "corrupt BSON");
   VALIDATE_TEST ("empty_key.bson",
                  BSON_VALIDATE_EMPTY_KEYS,
                  4,
                  BSON_VALIDATE_EMPTY_KEYS,
                  "empty key");
   VALIDATE_TEST (
      "test40.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST (
      "test41.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST (
      "test42.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST (
      "test43.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST (
      "test44.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST (
      "test45.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST (
      "test46.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST (
      "test47.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST (
      "test48.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST (
      "test49.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST ("test50.bson",
                  BSON_VALIDATE_NONE,
                  10,
                  BSON_VALIDATE_NONE,
                  "corrupt code-with-scope");
   VALIDATE_TEST ("test51.bson",
                  BSON_VALIDATE_NONE,
                  10,
                  BSON_VALIDATE_NONE,
                  "corrupt code-with-scope");
   VALIDATE_TEST (
      "test52.bson", BSON_VALIDATE_NONE, 9, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST (
      "test53.bson", BSON_VALIDATE_NONE, 6, BSON_VALIDATE_NONE, "corrupt BSON");
   VALIDATE_TEST ("test54.bson",
                  BSON_VALIDATE_NONE,
                   12,
                   BSON_VALIDATE_NONE,
                   "corrupt BSON");
   VALIDATE_TEST ("test59.bson",
                  BSON_VALIDATE_NONE,
                  9,
                  BSON_VALIDATE_NONE,
                  "corrupt BSON");
 
    /* DBRef validation */
    b = BCON_NEW ("my_dbref",
                 "{",
                 "$ref",
                 BCON_UTF8 ("collection"),
                 "$id",
                 BCON_INT32 (1),
                 "}");
   BSON_ASSERT (bson_validate_with_error (b, BSON_VALIDATE_NONE, &error));
   BSON_ASSERT (
      bson_validate_with_error (b, BSON_VALIDATE_DOLLAR_KEYS, &error));
   bson_destroy (b);

   /* needs "$ref" before "$id" */
   b = BCON_NEW ("my_dbref", "{", "$id", BCON_INT32 (1), "}");
   BSON_ASSERT (bson_validate_with_error (b, BSON_VALIDATE_NONE, &error));
   BSON_ASSERT (
      !bson_validate_with_error (b, BSON_VALIDATE_DOLLAR_KEYS, &error));
   ASSERT_ERROR_CONTAINS (error,
                          BSON_ERROR_INVALID,
                          BSON_VALIDATE_DOLLAR_KEYS,
                          "keys cannot begin with \"$\": \"$id\"");
   bson_destroy (b);

   /* two $refs */
   b = BCON_NEW ("my_dbref",
                 "{",
                 "$ref",
                 BCON_UTF8 ("collection"),
                 "$ref",
                 BCON_UTF8 ("collection"),
                 "}");
   BSON_ASSERT (bson_validate_with_error (b, BSON_VALIDATE_NONE, &error));
   BSON_ASSERT (
      !bson_validate_with_error (b, BSON_VALIDATE_DOLLAR_KEYS, &error));
   ASSERT_ERROR_CONTAINS (error,
                          BSON_ERROR_INVALID,
                          BSON_VALIDATE_DOLLAR_KEYS,
                          "keys cannot begin with \"$\": \"$ref\"");
   bson_destroy (b);

   /* must not contain invalid key like "extra" */
   b = BCON_NEW ("my_dbref",
                 "{",
                 "$ref",
                 BCON_UTF8 ("collection"),
                 "extra",
                 BCON_INT32 (2),
                 "$id",
                 BCON_INT32 (1),
                 "}");
   BSON_ASSERT (bson_validate_with_error (b, BSON_VALIDATE_NONE, &error));
   BSON_ASSERT (
      !bson_validate_with_error (b, BSON_VALIDATE_DOLLAR_KEYS, &error));
   ASSERT_ERROR_CONTAINS (error,
                          BSON_ERROR_INVALID,
                          BSON_VALIDATE_DOLLAR_KEYS,
                          "invalid key within DBRef subdocument: \"extra\"");
   bson_destroy (b);

#undef VALIDATE_TEST
}
