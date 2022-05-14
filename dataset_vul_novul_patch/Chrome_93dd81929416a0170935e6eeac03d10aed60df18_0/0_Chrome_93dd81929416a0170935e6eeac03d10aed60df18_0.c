static bool NPN_RemoveProperty(NPP npp, NPObject* npobj, NPIdentifier propertyName)
static bool NPN_RemoveProperty(NPP, NPObject* npObject, NPIdentifier propertyName)
 {
    if (npObject->_class->removeProperty)
        return npObject->_class->removeProperty(npObject, propertyName);

     return false;
 }
