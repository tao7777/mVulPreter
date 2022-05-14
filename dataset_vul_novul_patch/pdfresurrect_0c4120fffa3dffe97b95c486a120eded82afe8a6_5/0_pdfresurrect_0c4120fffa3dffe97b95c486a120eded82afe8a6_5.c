static pdf_creator_t *new_creator(int *n_elements)
{
    pdf_creator_t *daddy;

    static const pdf_creator_t creator_template[] = 
    {
        {"Title",        ""},
        {"Author",       ""},
        {"Subject",      ""},
        {"Keywords",     ""},
        {"Creator",      ""},
        {"Producer",     ""},
        {"CreationDate", ""},
        {"ModDate",      ""},
         {"Trapped",      ""},
     };
 
    daddy = safe_calloc(sizeof(creator_template));
     memcpy(daddy, creator_template, sizeof(creator_template));
 
     if (n_elements)
      *n_elements = sizeof(creator_template) / sizeof(creator_template[0]);

    return daddy;
}
