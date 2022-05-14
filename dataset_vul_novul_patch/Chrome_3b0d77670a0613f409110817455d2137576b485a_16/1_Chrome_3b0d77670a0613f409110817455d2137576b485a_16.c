const PPB_NaCl_Private* GetNaclInterface() {
  pp::Module *module = pp::Module::Get();
  CHECK(module);
  return static_cast<const PPB_NaCl_Private*>(
      module->GetBrowserInterface(PPB_NACL_PRIVATE_INTERFACE));
}
