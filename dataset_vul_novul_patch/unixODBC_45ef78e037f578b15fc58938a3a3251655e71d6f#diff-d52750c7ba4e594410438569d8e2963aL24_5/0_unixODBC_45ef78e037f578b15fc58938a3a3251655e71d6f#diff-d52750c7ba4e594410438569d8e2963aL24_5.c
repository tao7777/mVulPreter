SQLWCHAR* _single_string_alloc_and_expand( LPCSTR in )
{
    SQLWCHAR *chr;
    int len = 0;
 
     if ( !in )
     {
        return NULL;
     }
 
     while ( in[ len ] != 0 )
    {
        len ++;
    }

    chr = malloc( sizeof( SQLWCHAR ) * ( len + 1 ));

    len = 0;
    while ( in[ len ] != 0 )
    {
        chr[ len ] = in[ len ];
        len ++;
    }
    chr[ len ++ ] = 0;

    return chr;
}
