 pch_write_line (lin line, FILE *file)
 {
  bool after_newline = (p_len[line] > 0) && (p_line[line][p_len[line] - 1] == '\n');
   if (! fwrite (p_line[line], sizeof (*p_line[line]), p_len[line], file))
     write_fatal ();
   return after_newline;
}
