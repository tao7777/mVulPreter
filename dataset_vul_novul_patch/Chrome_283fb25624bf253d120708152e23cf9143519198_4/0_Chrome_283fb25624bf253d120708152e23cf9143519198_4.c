 pp::Var Invoke(Plugin* plugin,
                CallType call_type,
               const nacl::string& call_name,
                const char* caller,
                const std::vector<pp::Var>& args,
                pp::Var* exception) {
  uintptr_t id = plugin->browser_interface()->StringToIdentifier(call_name);

  SrpcParams params;
  NaClSrpcArg** inputs = params.ins();
  NaClSrpcArg** outputs = params.outs();
  if (!plugin->InitParams(id, call_type, &params)) {
    return Error(call_name, caller,
                 "srpc parameter initialization failed", exception);
  }
  uint32_t input_length = params.InputLength();
  int32_t output_length = params.OutputLength();
  PLUGIN_PRINTF(("ScriptableHandle::%s (initialized %"NACL_PRIu32" ins, %"
                 NACL_PRIu32" outs)\n", caller, input_length, output_length));

  if (args.size() != params.SignatureLength()) {
    return Error(call_name, caller,
                 "incompatible srpc parameter list", exception);
  }
  PLUGIN_PRINTF(("ScriptableHandle::%s (verified signature)\n", caller));

  if (input_length > 0) {
    assert(call_type != PROPERTY_GET);  // expect no inputs for "get"
    for (int i = 0; (i < NACL_SRPC_MAX_ARGS) && (inputs[i] != NULL); ++i) {
      if (!PPVarToNaClSrpcArg(args[i], inputs[i], exception)) {
        return Error(call_name, caller,
                     "srpc input marshalling failed", exception);
      }
    }
  }
  if (call_type == PROPERTY_SET) assert(input_length == 1);
  PLUGIN_PRINTF(("ScriptableHandle::%s (marshalled inputs)\n", caller));

  if (args.size() > input_length) {
    for (int i = 0; (i < NACL_SRPC_MAX_ARGS) && (outputs[i] != NULL); ++i) {
      if (!PPVarToAllocateNaClSrpcArg(args[input_length + i],
                                      outputs[i], exception)) {
        return Error(call_name, caller, "srpc output array allocation failed",
                     exception);
      }
    }
  }
  PLUGIN_PRINTF(("ScriptableHandle::%s (output array allocation done)\n",
                 caller));

  if (!plugin->Invoke(id, call_type, &params)) {
    nacl::string err = nacl::string(caller) + "('" + call_name + "') failed\n";
    if (params.exception_string() != NULL) {
      err = params.exception_string();
    }
    *exception = pp::Var(err.c_str());
    return Error(call_name, caller, "invocation failed", exception);
  }
  PLUGIN_PRINTF(("ScriptableHandle::%s (invocation done)\n", caller));

  pp::Var retvar;
  if (output_length > 0) {
    assert(call_type != PROPERTY_SET);  // expect no outputs for "set"
    retvar = NaClSrpcArgToPPVar(outputs[0], plugin, exception);
    if (output_length > 1) {
      ArrayPpapi* array = new(std::nothrow) ArrayPpapi(plugin);
      if (array == NULL) {
        *exception = pp::Var("failed to allocate output array");
      } else {
        array->SetProperty(pp::Var(0), retvar, exception);
        for (int32_t i = 1; i < output_length; ++i) {
          pp::Var v = NaClSrpcArgToPPVar(outputs[i], plugin, exception);
          array->SetProperty(pp::Var(i), v, exception);
        }
      }

      retvar = pp::VarPrivate(plugin, array);
    }
    if (!exception->is_undefined()) {
      return Error(call_name, caller, "srpc output marshalling failed",
                   exception);
    }
  }
  if (call_type == PROPERTY_GET) assert(output_length == 1);
  return retvar;
}
