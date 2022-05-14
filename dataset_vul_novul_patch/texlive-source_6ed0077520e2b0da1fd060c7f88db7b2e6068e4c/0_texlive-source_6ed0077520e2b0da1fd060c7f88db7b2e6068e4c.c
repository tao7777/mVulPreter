static void t1_check_unusual_charstring(void)
{
    char *p = strstr(t1_line_array, charstringname) + strlen(charstringname);
    int i;
    /* if no number follows "/CharStrings", let's read the next line */
    if (sscanf(p, "%i", &i) != 1) {
        /* pdftex_warn("no number found after `%s', I assume it's on the next line",
                    charstringname); */
        strcpy(t1_buf_array, t1_line_array);

        /* t1_getline always appends EOL to t1_line_array; let's change it to
         * space before appending the next line
         */
         *(strend(t1_buf_array) - 1) = ' ';
 
         t1_getline();
        alloc_array(t1_buf, strlen(t1_line_array) + strlen(t1_buf_array) + 1, T1_BUF_SIZE);
         strcat(t1_buf_array, t1_line_array);
        alloc_array(t1_line, strlen(t1_buf_array) + 1, T1_BUF_SIZE);
         strcpy(t1_line_array, t1_buf_array);
         t1_line_ptr = eol(t1_line_array);
     }
}
