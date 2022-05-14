static void addModuleArgument(sqlite3 *db, Table *pTable, char *zArg){
  int nBytes = sizeof(char *)*(2+pTable->nModuleArg);
   char **azModuleArg;
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
