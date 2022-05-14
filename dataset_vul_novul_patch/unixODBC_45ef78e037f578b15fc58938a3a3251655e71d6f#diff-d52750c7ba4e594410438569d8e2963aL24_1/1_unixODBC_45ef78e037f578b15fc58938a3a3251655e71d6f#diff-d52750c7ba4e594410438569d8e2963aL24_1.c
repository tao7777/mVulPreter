static int ExecuteSQL( SQLHDBC hDbc, char *szSQL, char cDelimiter, int bColumnNames, int bHTMLTable )
{
    SQLHSTMT        hStmt;
    SQLTCHAR        szSepLine[32001];   
    SQLTCHAR        szUcSQL[32001]; 
    SQLSMALLINT     cols;
    SQLINTEGER      ret;
    SQLLEN          nRows                   = 0;

    szSepLine[ 0 ] = 0;

    ansi_to_unicode( szSQL, szUcSQL );

    /****************************
     * EXECUTE SQL
     ***************************/
    if ( SQLAllocStmt( hDbc, &hStmt ) != SQL_SUCCESS )
    {
        if ( bVerbose ) DumpODBCLog( hEnv, hDbc, 0 );
        fprintf( stderr, "[ISQL]ERROR: Could not SQLAllocStmt\n" );
        return 0;
    }

    if ( buseED ) {
        ret = SQLExecDirect( hStmt, szUcSQL, SQL_NTS );

        if ( ret == SQL_NO_DATA )
        {
            fprintf( stderr, "[ISQL]INFO: SQLExecDirect returned SQL_NO_DATA\n" );
        }
        else if ( ret == SQL_SUCCESS_WITH_INFO )
        {
            if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
            fprintf( stderr, "[ISQL]INFO: SQLExecDirect returned SQL_SUCCESS_WITH_INFO\n" );
        }
        else if ( ret != SQL_SUCCESS )
        {
             if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
             fprintf( stderr, "[ISQL]ERROR: Could not SQLExecDirect\n" );
             SQLFreeStmt( hStmt, SQL_DROP );
            free(szSepLine);
             return 0;
         }
     }
    else {
        if ( SQLPrepare( hStmt, szUcSQL, SQL_NTS ) != SQL_SUCCESS )
        {
            if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
            fprintf( stderr, "[ISQL]ERROR: Could not SQLPrepare\n" );
            SQLFreeStmt( hStmt, SQL_DROP );
            return 0;
        }
    
        ret =  SQLExecute( hStmt );
    
        if ( ret == SQL_NO_DATA )
        {
            fprintf( stderr, "[ISQL]INFO: SQLExecute returned SQL_NO_DATA\n" );
        }
        else if ( ret == SQL_SUCCESS_WITH_INFO )
        {
            if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
            fprintf( stderr, "[ISQL]INFO: SQLExecute returned SQL_SUCCESS_WITH_INFO\n" );
        }
        else if ( ret != SQL_SUCCESS )
        {
            if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
            fprintf( stderr, "[ISQL]ERROR: Could not SQLExecute\n" );
            SQLFreeStmt( hStmt, SQL_DROP );
            return 0;
        }
    }

    do 
    {
        /*
         * check to see if it has generated a result set
         */

        if ( SQLNumResultCols( hStmt, &cols ) != SQL_SUCCESS )
        {
            if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
            fprintf( stderr, "[ISQL]ERROR: Could not SQLNumResultCols\n" );
            SQLFreeStmt( hStmt, SQL_DROP );
            return 0;
        }

        if ( cols > 0 )
        {
            /****************************
             * WRITE HEADER
             ***************************/
            if ( bHTMLTable )
                WriteHeaderHTMLTable( hStmt );
            else if ( cDelimiter == 0 )
                UWriteHeaderNormal( hStmt, szSepLine );
            else if ( cDelimiter && bColumnNames )
                WriteHeaderDelimited( hStmt, cDelimiter );

            /****************************
             * WRITE BODY
             ***************************/
            if ( bHTMLTable )
                WriteBodyHTMLTable( hStmt );
            else if ( cDelimiter == 0 )
                nRows = WriteBodyNormal( hStmt );
            else
                WriteBodyDelimited( hStmt, cDelimiter );
        }

        /****************************
         * WRITE FOOTER
         ***************************/
        if ( bHTMLTable )
            WriteFooterHTMLTable( hStmt );
        else if ( cDelimiter == 0 )
            UWriteFooterNormal( hStmt, szSepLine, nRows );
    }
    while ( SQL_SUCCEEDED( SQLMoreResults( hStmt )));

    /****************************
     * CLEANUP
     ***************************/
    SQLFreeStmt( hStmt, SQL_DROP );

    return 1;
}
