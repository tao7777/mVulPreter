 int main(int argc, char *argv[])
 {
   int ret_value = 0;
    libettercap_init();
    ef_globals_alloc();
    select_text_interface();
   libettercap_ui_init();
   /* etterfilter copyright */
   fprintf(stdout, "\n" EC_COLOR_BOLD "%s %s" EC_COLOR_END " copyright %s %s\n\n", 
                      PROGRAM, EC_VERSION, EC_COPYRIGHT, EC_AUTHORS);
 
   /* initialize the line number */
   EF_GBL->lineno = 1;
  
   /* getopt related parsing...  */
   parse_options(argc, argv);

   /* set the input for source file */
   if (EF_GBL_OPTIONS->source_file) {
      yyin = fopen(EF_GBL_OPTIONS->source_file, "r");
      if (yyin == NULL)
         FATAL_ERROR("Input file not found !");
   } else {
      FATAL_ERROR("No source file.");
   }

   /* no buffering */
   setbuf(yyin, NULL);
   setbuf(stdout, NULL);
   setbuf(stderr, NULL);

   
   /* load the tables in etterfilter.tbl */
   load_tables();
   /* load the constants in etterfilter.cnt */
   load_constants();

   /* print the message */
   fprintf(stdout, "\n Parsing source file \'%s\' ", EF_GBL_OPTIONS->source_file);
   fflush(stdout);

   ef_debug(1, "\n");

   /* begin the parsing */
   if (yyparse() == 0)
      fprintf(stdout, " done.\n\n");
   else
       fprintf(stdout, "\n\nThe script contains errors...\n\n");
   
    /* write to file */
   ret_value = write_output();
   if (ret_value == -E_NOTHANDLED)
      FATAL_ERROR("Cannot write output file (%s): the filter is not correctly handled.", EF_GBL_OPTIONS->output_file);
   else if (ret_value == -E_INVALID)
      FATAL_ERROR("Cannot write output file (%s): the filter format is not correct. ", EF_GBL_OPTIONS->output_file);

    ef_globals_free();
    return 0;
 }
