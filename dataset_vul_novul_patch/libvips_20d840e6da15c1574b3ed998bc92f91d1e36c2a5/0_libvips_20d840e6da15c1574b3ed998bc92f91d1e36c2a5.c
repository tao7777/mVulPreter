vips_foreign_load_start( VipsImage *out, void *a, void *b )
{
 	VipsForeignLoad *load = VIPS_FOREIGN_LOAD( b );
 	VipsForeignLoadClass *class = VIPS_FOREIGN_LOAD_GET_CLASS( load );
 
	/* If this start has failed before in another thread, we can fail now.
	 */
	if( load->error )
		return( NULL );

 	if( !load->real ) {
 		if( !(load->real = vips_foreign_load_temp( load )) )
 			return( NULL );

#ifdef DEBUG
		printf( "vips_foreign_load_start: triggering ->load()\n" );
#endif /*DEBUG*/

		/* Read the image in. This may involve a long computation and
		 * will finish with load->real holding the decompressed image. 
		 *
		 * We want our caller to be able to see this computation on
		 * @out, so eval signals on ->real need to appear on ->out.
		 */
		load->real->progress_signal = load->out;

		/* Note the load object on the image. Loaders can use 
		 * this to signal invalidate if they hit a load error. See
		 * vips_foreign_load_invalidate() below.
		 */
 		g_object_set_qdata( G_OBJECT( load->real ), 
 			vips__foreign_load_operation, load ); 
 
		/* Load the image and check the result.
		 *
		 * ->header() read the header into @out, load has read the
 		 * image into @real. They must match exactly in size, bands,
 		 * format and coding for the copy to work.  
 		 *
 		 * Some versions of ImageMagick give different results between
 		 * Ping and Load for some formats, for example.
		 *
		 * If the load fails, we need to stop
 		 */
		if( class->load( load ) ||
			vips_image_pio_input( load->real ) || 
			vips_foreign_load_iscompat( load->real, out ) ) {
			vips_operation_invalidate( VIPS_OPERATION( load ) ); 
			load->error = TRUE;

 			return( NULL );
		}
 
 		/* We have to tell vips that out depends on real. We've set
 		 * the demand hint below, but not given an input there.
		 */
		vips_image_pipelinev( load->out, load->out->dhint, 
			load->real, NULL );
	}

	return( vips_region_new( load->real ) );
}
