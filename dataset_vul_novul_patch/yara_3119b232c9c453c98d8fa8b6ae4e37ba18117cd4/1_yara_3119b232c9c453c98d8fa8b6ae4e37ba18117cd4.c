static void re_yyensure_buffer_stack (yyscan_t yyscanner)
{
	yy_size_t num_to_alloc;
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;

	if (!yyg->yy_buffer_stack) {

		/* First allocation is just for 2 elements, since we don't know if this
		 * scanner will even need a stack. We use 2 instead of 1 to avoid an
		 * immediate realloc on the next call.
         */
		num_to_alloc = 1; // After all that talk, this was set to 1 anyways...
		yyg->yy_buffer_stack = (struct yy_buffer_state**)re_yyalloc
								(num_to_alloc * sizeof(struct yy_buffer_state*)
 								, yyscanner);
 		if ( ! yyg->yy_buffer_stack )
 			YY_FATAL_ERROR( "out of dynamic memory in re_yyensure_buffer_stack()" );
 		memset(yyg->yy_buffer_stack, 0, num_to_alloc * sizeof(struct yy_buffer_state*));
 		yyg->yy_buffer_stack_max = num_to_alloc;
 		yyg->yy_buffer_stack_top = 0;
 		return;
	}

	if (yyg->yy_buffer_stack_top >= (yyg->yy_buffer_stack_max) - 1){

		/* Increase the buffer to prepare for a possible push. */
		yy_size_t grow_size = 8 /* arbitrary grow size */;

		num_to_alloc = yyg->yy_buffer_stack_max + grow_size;
		yyg->yy_buffer_stack = (struct yy_buffer_state**)re_yyrealloc
								(yyg->yy_buffer_stack,
								num_to_alloc * sizeof(struct yy_buffer_state*)
								, yyscanner);
		if ( ! yyg->yy_buffer_stack )
			YY_FATAL_ERROR( "out of dynamic memory in re_yyensure_buffer_stack()" );

		/* zero only the new slots.*/
		memset(yyg->yy_buffer_stack + yyg->yy_buffer_stack_max, 0, grow_size * sizeof(struct yy_buffer_state*));
		yyg->yy_buffer_stack_max = num_to_alloc;
	}
}
