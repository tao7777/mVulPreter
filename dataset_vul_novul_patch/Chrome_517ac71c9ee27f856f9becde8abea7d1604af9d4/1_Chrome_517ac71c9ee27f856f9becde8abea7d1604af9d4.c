static void addArgumentToVtab(Parse *pParse){
  if( pParse->sArg.z && pParse->pNewTable ){
     const char *z = (const char*)pParse->sArg.z;
     int n = pParse->sArg.n;
     sqlite3 *db = pParse->db;
    addModuleArgument(db, pParse->pNewTable, sqlite3DbStrNDup(db, z, n));
   }
 }
