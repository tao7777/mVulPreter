find_insert(png_const_charp what, png_charp param)
{
   png_uint_32 chunk = 0;
   png_charp parameter_list[1024];
 int i, nparams;

 /* Assemble the chunk name */
 for (i=0; i<4; ++i)
 {
 char ch = what[i];

 if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
         chunk = (chunk << 8) + what[i];

 else
 break;
 }

 if (i < 4 || what[4] != 0)
 {
      fprintf(stderr, "makepng --insert \"%s\": invalid chunk name\n", what);
      exit(1);
 }

 /* Assemble the parameter list. */
   nparams = find_parameters(what, param, parameter_list, 1024);

#  define CHUNK(a,b,c,d) (((a)<<24)+((b)<<16)+((c)<<8)+(d))

 switch (chunk)
 {
 case CHUNK(105,67,67,80): /* iCCP */
 if (nparams == 2)
 return make_insert(what, insert_iCCP, nparams, parameter_list);
 break;

 case CHUNK(116,69,88,116): /* tEXt */
 if (nparams == 2)
 return make_insert(what, insert_tEXt, nparams, parameter_list);
 break;

 case CHUNK(122,84,88,116): /* zTXt */
 if (nparams == 2)
 return make_insert(what, insert_zTXt, nparams, parameter_list);
 break;

 case CHUNK(105,84,88,116): /* iTXt */
 if (nparams == 4)
 return make_insert(what, insert_iTXt, nparams, parameter_list);
 break;

 case CHUNK(104,73,83,84): /* hIST */
 if (nparams <= 256)

             return make_insert(what, insert_hIST, nparams, parameter_list);
          break;
 
 #if 0
       case CHUNK(115,80,76,84):  /* sPLT */
          return make_insert(what, insert_sPLT, nparams, parameter_list);
#endif

 default:
         fprintf(stderr, "makepng --insert \"%s\": unrecognized chunk name\n",
            what);
         exit(1);
 }

   bad_parameter_count(what, nparams);

    return NULL;
 }
