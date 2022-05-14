void _jsvTrace(JsVar *var, int indent, JsVar *baseVar, int level) {
#ifdef SAVE_ON_FLASH
  jsiConsolePrint("Trace unimplemented in this version.\n");
#else
  int i;
  for (i=0;i<indent;i++) jsiConsolePrint(" ");

  if (!var) {
    jsiConsolePrint("undefined");
    return;
  }

  jsvTraceLockInfo(var);

  int lowestLevel = _jsvTraceGetLowestLevel(baseVar, var);
  if (lowestLevel < level) {
    jsiConsolePrint("...\n");
    return;
  }

  if (jsvIsName(var)) jsiConsolePrint("Name ");

  char endBracket = ' ';
  if (jsvIsObject(var)) { jsiConsolePrint("Object { "); endBracket = '}'; }
  else if (jsvIsArray(var)) { jsiConsolePrintf("Array(%d) [ ", var->varData.integer); endBracket = ']'; }
  else if (jsvIsNativeFunction(var)) { jsiConsolePrintf("NativeFunction 0x%x (%d) { ", var->varData.native.ptr, var->varData.native.argTypes); endBracket = '}'; }
  else if (jsvIsFunction(var)) {
    jsiConsolePrint("Function { ");
    if (jsvIsFunctionReturn(var)) jsiConsolePrint("return ");
    endBracket = '}';
  } else if (jsvIsPin(var)) jsiConsolePrintf("Pin %d", jsvGetInteger(var));
  else if (jsvIsInt(var)) jsiConsolePrintf("Integer %d", jsvGetInteger(var));
  else if (jsvIsBoolean(var)) jsiConsolePrintf("Bool %s", jsvGetBool(var)?"true":"false");
   else if (jsvIsFloat(var)) jsiConsolePrintf("Double %f", jsvGetFloat(var));
   else if (jsvIsFunctionParameter(var)) jsiConsolePrintf("Param %q ", var);
   else if (jsvIsArrayBufferName(var)) jsiConsolePrintf("ArrayBufferName[%d] ", jsvGetInteger(var));
  else if (jsvIsArrayBuffer(var)) jsiConsolePrintf("%s ", jswGetBasicObjectName(var)?jswGetBasicObjectName(var):"unknown ArrayBuffer"); // way to get nice name
   else if (jsvIsString(var)) {
     size_t blocks = 1;
     if (jsvGetLastChild(var)) {
      JsVar *v = jsvLock(jsvGetLastChild(var));
      blocks += jsvCountJsVarsUsed(v);
      jsvUnLock(v);
    }
    if (jsvIsFlatString(var)) {
      blocks += jsvGetFlatStringBlocks(var);
    }
    jsiConsolePrintf("%sString [%d blocks] %q", jsvIsFlatString(var)?"Flat":(jsvIsNativeString(var)?"Native":""), blocks, var);
  } else {
    jsiConsolePrintf("Unknown %d", var->flags & (JsVarFlags)~(JSV_LOCK_MASK));
  }

  if (jsvIsNameInt(var)) {
    jsiConsolePrintf("= int %d\n", (int)jsvGetFirstChildSigned(var));
    return;
  } else if (jsvIsNameIntBool(var)) {
    jsiConsolePrintf("= bool %s\n", jsvGetFirstChild(var)?"true":"false");
    return;
  }

  if (jsvHasSingleChild(var)) {
    JsVar *child = jsvGetFirstChild(var) ? jsvLock(jsvGetFirstChild(var)) : 0;
    _jsvTrace(child, indent+2, baseVar, level+1);
    jsvUnLock(child);
  } else if (jsvHasChildren(var)) {
    JsvIterator it;
    jsvIteratorNew(&it, var, JSIF_DEFINED_ARRAY_ElEMENTS);
    bool first = true;
    while (jsvIteratorHasElement(&it) && !jspIsInterrupted()) {
      if (first) jsiConsolePrintf("\n");
      first = false;
      JsVar *child = jsvIteratorGetKey(&it);
      _jsvTrace(child, indent+2, baseVar, level+1);
      jsvUnLock(child);
      jsiConsolePrintf("\n");
      jsvIteratorNext(&it);
    }
    jsvIteratorFree(&it);
    if (!first)
      for (i=0;i<indent;i++) jsiConsolePrint(" ");
  }
  jsiConsolePrintf("%c", endBracket);
#endif
}
