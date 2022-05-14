zlib_end(struct zlib *zlib)
{
 /* Output the summary line now; this ensures a summary line always gets
    * output regardless of the manner of exit.
    */
 if (!zlib->global->quiet)
 {
 if (zlib->ok_bits < 16) /* stream was read ok */
 {
 const char *reason;

 if (zlib->cksum)
            reason = "CHK"; /* checksum error */

 else if (zlib->ok_bits > zlib->file_bits)
            reason = "TFB"; /* fixing a too-far-back error */

 else if (zlib->ok_bits == zlib->file_bits)
            reason = "OK ";

 else
            reason = "OPT"; /* optimizing window bits */

 /* SUMMARY FORMAT (for a successful zlib inflate):
          *
          * IDAT reason flevel file-bits ok-bits compressed uncompressed file
          */
         type_name(zlib->chunk->chunk_type, stdout);
         printf(" %s %s %d %d ", reason, zlib_flevel(zlib), zlib->file_bits,
            zlib->ok_bits);
         uarb_print(zlib->compressed_bytes, zlib->compressed_digits, stdout);
         putc(' ', stdout);
         uarb_print(zlib->uncompressed_bytes, zlib->uncompressed_digits,
            stdout);
         putc(' ', stdout);
         fputs(zlib->file->file_name, stdout);
         putc('\n', stdout);
 }

 else
 {
 /* This is a zlib read error; the chunk will be skipped.  For an IDAT
          * stream this will also cause a fatal read error (via stop()).
          *
          * SUMMARY FORMAT:
          *
          * IDAT SKP flevel file-bits z-rc compressed message file

           *
           * z-rc is the zlib failure code; message is the error message with
           * spaces replaced by '-'.  The compressed byte count indicates where
          * in the zlib stream the error occured.
           */
          type_name(zlib->chunk->chunk_type, stdout);
          printf(" SKP %s %d %s ", zlib_flevel(zlib), zlib->file_bits,
            zlib_rc(zlib));
         uarb_print(zlib->compressed_bytes, zlib->compressed_digits, stdout);
         putc(' ', stdout);
         emit_string(zlib->z.msg ? zlib->z.msg : "[no_message]", stdout);
         putc(' ', stdout);
         fputs(zlib->file->file_name, stdout);
         putc('\n', stdout);
 }
 }

 if (zlib->state >= 0)
 {
      zlib->rc = inflateEnd(&zlib->z);

 if (zlib->rc != Z_OK)
         zlib_message(zlib, 1/*unexpected*/);
 }

   CLEAR(*zlib);
}
