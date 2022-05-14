int rpc_type_of_NPNVariable(int variable)
{
  int type;
  switch (variable) {
  case NPNVjavascriptEnabledBool:
  case NPNVasdEnabledBool:
   case NPNVisOfflineBool:
   case NPNVSupportsXEmbedBool:
   case NPNVSupportsWindowless:
  case NPNVprivateModeBool:
  case NPNVsupportsAdvancedKeyHandling:
 	type = RPC_TYPE_BOOLEAN;
 	break;
   case NPNVToolkit:
  case NPNVnetscapeWindow:
	type = RPC_TYPE_UINT32;
	break;
  case NPNVWindowNPObject:
  case NPNVPluginElementNPObject:
	type = RPC_TYPE_NP_OBJECT;
	break;
  default:
	type = RPC_ERROR_GENERIC;
	break;
  }
  return type;
}
