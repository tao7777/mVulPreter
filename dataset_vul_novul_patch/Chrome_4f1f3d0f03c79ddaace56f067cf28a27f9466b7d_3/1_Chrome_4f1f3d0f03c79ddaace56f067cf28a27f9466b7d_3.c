bool GetPathFromHandle(HANDLE handle, std::wstring* path) {
   NtQueryObjectFunction NtQueryObject = NULL;
   ResolveNTFunctionPtr("NtQueryObject", &NtQueryObject);
 
  OBJECT_NAME_INFORMATION* name = NULL;
  ULONG size = 0;
   NTSTATUS status = NtQueryObject(handle, ObjectNameInformation, name, size,
                                   &size);

  scoped_ptr<OBJECT_NAME_INFORMATION> name_ptr;
  if (size) {
    name = reinterpret_cast<OBJECT_NAME_INFORMATION*>(new BYTE[size]);
    name_ptr.reset(name);

    status = NtQueryObject(handle, ObjectNameInformation, name, size, &size);
  }

  if (STATUS_SUCCESS != status)
    return false;

  path->assign(name->ObjectName.Buffer, name->ObjectName.Length /
                                        sizeof(name->ObjectName.Buffer[0]));
  return true;
}
