const char *string_of_NPNVariable(int variable)
{
  const char *str;

  switch (variable) {
#define _(VAL) case VAL: str = #VAL; break;
	_(NPNVxDisplay);
	_(NPNVxtAppContext);
	_(NPNVnetscapeWindow);
	_(NPNVjavascriptEnabledBool);
	_(NPNVasdEnabledBool);
	_(NPNVisOfflineBool);
	_(NPNVserviceManager);
	_(NPNVDOMElement);
	_(NPNVDOMWindow);
	_(NPNVToolkit);
	_(NPNVSupportsXEmbedBool);
 	_(NPNVWindowNPObject);
 	_(NPNVPluginElementNPObject);
 	_(NPNVSupportsWindowless);
 #undef _
   default:
 	switch (variable & 0xff) {
#define _(VAL, VAR) case VAL: str = #VAR; break
	  _(10, NPNVserviceManager);
	  _(11, NPNVDOMElement);
	  _(12, NPNVDOMWindow);
	  _(13, NPNVToolkit);
#undef _
	default:
	  str = "<unknown variable>";
	  break;
	}
	break;
  }

  return str;
}
