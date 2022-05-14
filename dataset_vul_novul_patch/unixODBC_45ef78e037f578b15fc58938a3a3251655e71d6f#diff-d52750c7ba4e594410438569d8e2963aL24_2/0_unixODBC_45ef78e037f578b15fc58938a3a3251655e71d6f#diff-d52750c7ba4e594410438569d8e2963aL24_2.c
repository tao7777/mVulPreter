char* _multi_string_alloc_and_copy( LPCWSTR in )
{
    char *chr;
    int len = 0;
 
     if ( !in )
     {
        return NULL;
     }
 
     while ( in[ len ] != 0 || in[ len + 1 ] != 0 )
    {
        len ++;
    }

    chr = malloc( len + 2 );

    len = 0;
    while ( in[ len ] != 0 || in[ len + 1 ] != 0 )
    {
        chr[ len ] = 0xFF & in[ len ];
        len ++;
    }
    chr[ len ++ ] = '\0';
    chr[ len ++ ] = '\0';

    return chr;
}
