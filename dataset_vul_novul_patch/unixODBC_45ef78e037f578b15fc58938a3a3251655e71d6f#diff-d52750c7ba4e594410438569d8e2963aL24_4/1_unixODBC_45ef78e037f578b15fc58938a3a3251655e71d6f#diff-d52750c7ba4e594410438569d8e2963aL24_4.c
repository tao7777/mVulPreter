char* _single_string_alloc_and_copy( LPCWSTR in )
{
    char *chr;
    int len = 0;
 
     if ( !in )
     {
        return in;
     }
 
     while ( in[ len ] != 0 )
    {
        len ++;
    }

    chr = malloc( len + 1 );

    len = 0;
    while ( in[ len ] != 0 )
    {
        chr[ len ] = 0xFF & in[ len ];
        len ++;
    }
    chr[ len ++ ] = '\0';

    return chr;
}
