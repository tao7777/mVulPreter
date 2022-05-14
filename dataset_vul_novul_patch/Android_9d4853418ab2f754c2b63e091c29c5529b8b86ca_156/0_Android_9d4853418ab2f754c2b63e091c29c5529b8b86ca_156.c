int main(int argc, char *argv[])
{
 FILE *fp_rd = stdin;
 FILE *fp_wr = stdout;
 FILE *fp_al = NULL;
  BOOL raw = TRUE;
  BOOL alpha = FALSE;
 int argi;

 for (argi = 1; argi < argc; argi++)
 {
 if (argv[argi][0] == '-')
 {
 switch (argv[argi][1])
 {
 case 'n':
          raw = FALSE;
 break;
 case 'r':
          raw = TRUE;
 break;
 case 'a':
          alpha = TRUE;
          argi++;

           if ((fp_al = fopen (argv[argi], "wb")) == NULL)
           {
             fprintf (stderr, "PNM2PNG\n");
            fprintf (stderr, "Error:  can not create alpha-channel file %s\n",
               argv[argi]);
             exit (1);
           }
           break;
 case 'h':
 case '?':
          usage();
          exit(0);
 break;
 default:
          fprintf (stderr, "PNG2PNM\n");
          fprintf (stderr, "Error:  unknown option %s\n", argv[argi]);
          usage();
          exit(1);
 break;
 } /* end switch */
 }
 else if (fp_rd == stdin)
 {
 if ((fp_rd = fopen (argv[argi], "rb")) == NULL)
 {
             fprintf (stderr, "PNG2PNM\n");
            fprintf (stderr, "Error:  file %s does not exist\n", argv[argi]);
            exit (1);
 }
 }
 else if (fp_wr == stdout)
 {
 if ((fp_wr = fopen (argv[argi], "wb")) == NULL)
 {
        fprintf (stderr, "PNG2PNM\n");
        fprintf (stderr, "Error:  can not create file %s\n", argv[argi]);
        exit (1);
 }
 }
 else
 {
      fprintf (stderr, "PNG2PNM\n");
      fprintf (stderr, "Error:  too many parameters\n");
      usage();
      exit(1);
 }
 } /* end for */

#ifdef __TURBOC__
 /* set stdin/stdout if required to binary */
 if (fp_rd == stdin)
 {
    setmode (STDIN, O_BINARY);
 }
 if ((raw) && (fp_wr == stdout))
 {
    setmode (STDOUT, O_BINARY);
 }
#endif

 /* call the conversion program itself */
 if (png2pnm (fp_rd, fp_wr, fp_al, raw, alpha) == FALSE)
 {
    fprintf (stderr, "PNG2PNM\n");
    fprintf (stderr, "Error:  unsuccessful conversion of PNG-image\n");
    exit(1);
 }

 /* close input file */
  fclose (fp_rd);
 /* close output file */
  fclose (fp_wr);
 /* close alpha file */
 if (alpha)
    fclose (fp_al);

 return 0;
}
