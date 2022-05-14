static void readpng2_end_callback(png_structp png_ptr, png_infop info_ptr)
{
    mainprog_info  *mainprog_ptr;


 /* retrieve the pointer to our special-purpose struct */

    mainprog_ptr = png_get_progressive_ptr(png_ptr);


 /* let the main program know that it should flush any buffered image
     * data to the display now and set a "done" flag or whatever, but note
     * that it SHOULD NOT DESTROY THE PNG STRUCTS YET--in other words, do
     * NOT call readpng2_cleanup() either here or in the finish_display()
     * routine; wait until control returns to the main program via
     * readpng2_decode_data() */

 (*mainprog_ptr->mainprog_finish_display)();


 
     /* all done */
 
     return;
 }
