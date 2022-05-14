static void* lookupOpenGLFunctionAddress(const char* functionName, bool* success = 0)
{
    if (success && !*success)
        return 0;

    void* target = getProcAddress(functionName);
    if (target)
        return target;

    String fullFunctionName(functionName);
    fullFunctionName.append("ARB");
    target = getProcAddress(fullFunctionName.utf8().data());
    if (target)
        return target;

    fullFunctionName = functionName;
    fullFunctionName.append("EXT");
    target = getProcAddress(fullFunctionName.utf8().data());

#if defined(GL_ES_VERSION_2_0)
     fullFunctionName = functionName;
     fullFunctionName.append("ANGLE");
     target = getProcAddress(fullFunctionName.utf8().data());
    if (target)
        return target;
 
     fullFunctionName = functionName;
     fullFunctionName.append("APPLE");
    target = getProcAddress(fullFunctionName.utf8().data());
#endif

    if (!target && success)
        *success = false;

    return target;
}
