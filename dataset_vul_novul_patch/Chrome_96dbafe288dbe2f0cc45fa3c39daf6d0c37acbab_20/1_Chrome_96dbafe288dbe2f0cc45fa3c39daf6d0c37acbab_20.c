xsltNumberFormatAlpha(xmlBufferPtr buffer,
		      double number,
		      int is_upper)
{
    char temp_string[sizeof(double) * CHAR_BIT * sizeof(xmlChar) + 1];
    char *pointer;
    int i;
     char *alpha_list;
     double alpha_size = (double)(sizeof(alpha_upper_list) - 1);
 
     /* Build buffer from back */
     pointer = &temp_string[sizeof(temp_string)];
     *(--pointer) = 0;
    alpha_list = (is_upper) ? alpha_upper_list : alpha_lower_list;

    for (i = 1; i < (int)sizeof(temp_string); i++) {
 	number--;
 	*(--pointer) = alpha_list[((int)fmod(number, alpha_size))];
 	number /= alpha_size;
	if (fabs(number) < 1.0)
 	    break; /* for */
     }
     xmlBufferCCat(buffer, pointer);
}
