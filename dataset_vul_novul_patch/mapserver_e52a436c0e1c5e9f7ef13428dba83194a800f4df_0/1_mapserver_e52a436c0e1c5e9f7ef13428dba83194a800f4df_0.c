char *FLTGetIsLikeComparisonCommonExpression(FilterEncodingNode *psFilterNode)
{
  const size_t bufferSize = 1024;
  char szBuffer[1024];
  char szTmp[256];
  char *pszValue = NULL;

  const char *pszWild = NULL;
  const char *pszSingle = NULL;
  const char *pszEscape = NULL;
  int  bCaseInsensitive = 0;
  FEPropertyIsLike* propIsLike;

  int nLength=0, i=0, iTmp=0;

  if (!psFilterNode || !psFilterNode->pOther || !psFilterNode->psLeftNode || !psFilterNode->psRightNode || !psFilterNode->psRightNode->pszValue)
    return NULL;

  propIsLike = (FEPropertyIsLike *)psFilterNode->pOther;
  pszWild = propIsLike->pszWildCard;
  pszSingle = propIsLike->pszSingleChar;
  pszEscape = propIsLike->pszEscapeChar;
  bCaseInsensitive = propIsLike->bCaseInsensitive;

  if (!pszWild || strlen(pszWild) == 0 || !pszSingle || strlen(pszSingle) == 0 || !pszEscape || strlen(pszEscape) == 0)
    return NULL;

  /* -------------------------------------------------------------------- */
  /*      Use operand with regular expressions.                           */
  /* -------------------------------------------------------------------- */
  szBuffer[0] = '\0';
  sprintf(szTmp, "%s", "(\"[");
  szTmp[4] = '\0';

  strlcat(szBuffer, szTmp, bufferSize);

  /* attribute */
  strlcat(szBuffer, psFilterNode->psLeftNode->pszValue, bufferSize);
  szBuffer[strlen(szBuffer)] = '\0';

  /* #3521 */
  if (bCaseInsensitive == 1)
    sprintf(szTmp, "%s", "]\" ~* \"");
  else
    sprintf(szTmp, "%s", "]\" ~ \"");
  szTmp[7] = '\0';
  strlcat(szBuffer, szTmp, bufferSize);
  szBuffer[strlen(szBuffer)] = '\0';
 
   pszValue = psFilterNode->psRightNode->pszValue;
   nLength = strlen(pszValue);
 
   iTmp =0;
   if (nLength > 0 && pszValue[0] != pszWild[0] && pszValue[0] != pszSingle[0] && pszValue[0] != pszEscape[0]) {
    szTmp[iTmp]= '^';
    iTmp++;
  }
  for (i=0; i<nLength; i++) {
    if (pszValue[i] != pszWild[0] && pszValue[i] != pszSingle[0] && pszValue[i] != pszEscape[0]) {
      szTmp[iTmp] = pszValue[i];
      iTmp++;
      szTmp[iTmp] = '\0';
    } else if (pszValue[i] == pszSingle[0]) {
      szTmp[iTmp] = '.';
      iTmp++;
      szTmp[iTmp] = '\0';
    } else if (pszValue[i] == pszEscape[0]) {
      szTmp[iTmp] = '\\';
      iTmp++;
      szTmp[iTmp] = '\0';
    } else if (pszValue[i] == pszWild[0]) {
      szTmp[iTmp++] = '.';
      szTmp[iTmp++] = '*';
      szTmp[iTmp] = '\0';
    }
  }
  szTmp[iTmp] = '"';
  szTmp[++iTmp] = '\0';

  strlcat(szBuffer, szTmp, bufferSize);
  strlcat(szBuffer, ")", bufferSize);
  return msStrdup(szBuffer);
}
