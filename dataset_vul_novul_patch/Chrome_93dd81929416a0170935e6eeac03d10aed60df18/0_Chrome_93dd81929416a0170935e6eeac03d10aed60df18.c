NPClass* NPJSObject::npClass()
{
    static NPClass npClass = {
        NP_CLASS_STRUCT_VERSION,
        NP_Allocate,
        NP_Deallocate,
        0,
        NP_HasMethod,
        NP_Invoke,
        NP_InvokeDefault,
         NP_HasProperty,
         NP_GetProperty,
         NP_SetProperty,
        NP_RemoveProperty,
         NP_Enumerate,
         NP_Construct
     };

    return &npClass;
}
