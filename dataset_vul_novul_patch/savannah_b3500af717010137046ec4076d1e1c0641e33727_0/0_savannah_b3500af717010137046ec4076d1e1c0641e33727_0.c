  FT_Bitmap_Copy( FT_Library        library,
                  const FT_Bitmap  *source,
                  FT_Bitmap        *target)
  {
    FT_Memory  memory = library->memory;
    FT_Error   error  = FT_Err_Ok;
    FT_Int     pitch  = source->pitch;
    FT_ULong   size;


    if ( source == target )
      return FT_Err_Ok;

    if ( source->buffer == NULL )
    {
      *target = *source;

      return FT_Err_Ok;
    }
 
     if ( pitch < 0 )
       pitch = -pitch;
    size = (FT_ULong)pitch * source->rows;
 
     if ( target->buffer )
     {
      FT_Int    target_pitch = target->pitch;
      FT_ULong  target_size;

 
       if ( target_pitch < 0  )
         target_pitch = -target_pitch;
      target_size = (FT_ULong)target_pitch * target->rows;
 
       if ( target_size != size )
         (void)FT_QREALLOC( target->buffer, target_size, size );
    }
    else
      (void)FT_QALLOC( target->buffer, size );

    if ( !error )
    {
      unsigned char *p;


      p = target->buffer;
      *target = *source;
      target->buffer = p;

      FT_MEM_COPY( target->buffer, source->buffer, size );
    }

    return error;
  }
