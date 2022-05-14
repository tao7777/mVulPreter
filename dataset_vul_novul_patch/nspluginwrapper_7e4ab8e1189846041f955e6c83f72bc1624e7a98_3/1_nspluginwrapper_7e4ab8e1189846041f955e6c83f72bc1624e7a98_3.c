const char *string_of_NPPVariable(int variable)
{
  const char *str;

  switch (variable) {
#define _(VAL) case VAL: str = #VAL; break;
	_(NPPVpluginNameString);
	_(NPPVpluginDescriptionString);
	_(NPPVpluginWindowBool);
	_(NPPVpluginTransparentBool);
	_(NPPVjavaClass);
	_(NPPVpluginWindowSize);
	_(NPPVpluginTimerInterval);
	_(NPPVpluginScriptableInstance);
	_(NPPVpluginScriptableIID);
	_(NPPVjavascriptPushCallerBool);
	_(NPPVpluginKeepLibraryInMemory);
 	_(NPPVpluginNeedsXEmbed);
 	_(NPPVpluginScriptableNPObject);
 	_(NPPVformValue);
 #undef _
   default:
 	switch (variable & 0xff) {
#define _(VAL, VAR) case VAL: str = #VAR; break
	  _(10, NPPVpluginScriptableInstance);
#undef _
	default:
	  str = "<unknown variable>";
	  break;
	}
	break;
  }

  return str;
}
