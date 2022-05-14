static void addModuleArgument(sqlite3 *db, Table *pTable, char *zArg){
static void addModuleArgument(Parse *pParse, Table *pTable, char *zArg){
  sqlite3_int64 nBytes = sizeof(char *)*(2+pTable->nModuleArg);
   char **azModuleArg;
  sqlite3 *db = pParse->db;
  if( pTable->nModuleArg+3>=db->aLimit[SQLITE_LIMIT_COLUMN] ){
    sqlite3ErrorMsg(pParse, "too many columns on %s", pTable->zName);
  }
   azModuleArg = sqlite3DbRealloc(db, pTable->azModuleArg, nBytes);
   if( azModuleArg==0 ){
     sqlite3DbFree(db, zArg);
  }else{
    int i = pTable->nModuleArg++;
    azModuleArg[i] = zArg;
    azModuleArg[i+1] = 0;
    pTable->azModuleArg = azModuleArg;
  }
}
