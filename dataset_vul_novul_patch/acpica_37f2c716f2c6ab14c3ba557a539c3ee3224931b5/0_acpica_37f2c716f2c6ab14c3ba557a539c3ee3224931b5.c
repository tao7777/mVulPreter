AcpiNsEvaluate (
    ACPI_EVALUATE_INFO      *Info)
{
    ACPI_STATUS             Status;


    ACPI_FUNCTION_TRACE (NsEvaluate);


    if (!Info)
    {
        return_ACPI_STATUS (AE_BAD_PARAMETER);
    }

    if (!Info->Node)
    {
        /*
         * Get the actual namespace node for the target object if we
         * need to. Handles these cases:
         *
         * 1) Null node, valid pathname from root (absolute path)
         * 2) Node and valid pathname (path relative to Node)
         * 3) Node, Null pathname
         */
        Status = AcpiNsGetNode (Info->PrefixNode, Info->RelativePathname,
            ACPI_NS_NO_UPSEARCH, &Info->Node);
        if (ACPI_FAILURE (Status))
        {
            return_ACPI_STATUS (Status);
        }
    }

    /*
     * For a method alias, we must grab the actual method node so that
     * proper scoping context will be established before execution.
     */
    if (AcpiNsGetType (Info->Node) == ACPI_TYPE_LOCAL_METHOD_ALIAS)
    {
        Info->Node = ACPI_CAST_PTR (
            ACPI_NAMESPACE_NODE, Info->Node->Object);
    }

    /* Complete the info block initialization */

    Info->ReturnObject = NULL;
    Info->NodeFlags = Info->Node->Flags;
    Info->ObjDesc = AcpiNsGetAttachedObject (Info->Node);

    ACPI_DEBUG_PRINT ((ACPI_DB_NAMES, "%s [%p] Value %p\n",
        Info->RelativePathname, Info->Node,
        AcpiNsGetAttachedObject (Info->Node)));

    /* Get info if we have a predefined name (_HID, etc.) */

    Info->Predefined = AcpiUtMatchPredefinedMethod (Info->Node->Name.Ascii);

    /* Get the full pathname to the object, for use in warning messages */

    Info->FullPathname = AcpiNsGetNormalizedPathname (Info->Node, TRUE);
    if (!Info->FullPathname)
    {
        return_ACPI_STATUS (AE_NO_MEMORY);
    }

    /* Count the number of arguments being passed in */

    Info->ParamCount = 0;
    if (Info->Parameters)
    {
        while (Info->Parameters[Info->ParamCount])
        {
            Info->ParamCount++;
        }

        /* Warn on impossible argument count */

        if (Info->ParamCount > ACPI_METHOD_NUM_ARGS)
        {
            ACPI_WARN_PREDEFINED ((AE_INFO, Info->FullPathname, ACPI_WARN_ALWAYS,
                "Excess arguments (%u) - using only %u",
                Info->ParamCount, ACPI_METHOD_NUM_ARGS));

            Info->ParamCount = ACPI_METHOD_NUM_ARGS;
        }
    }

    /*
     * For predefined names: Check that the declared argument count
     * matches the ACPI spec -- otherwise this is a BIOS error.
     */
    AcpiNsCheckAcpiCompliance (Info->FullPathname, Info->Node,
        Info->Predefined);

    /*
     * For all names: Check that the incoming argument count for
     * this method/object matches the actual ASL/AML definition.
     */
    AcpiNsCheckArgumentCount (Info->FullPathname, Info->Node,
        Info->ParamCount, Info->Predefined);

    /* For predefined names: Typecheck all incoming arguments */

    AcpiNsCheckArgumentTypes (Info);

    /*
     * Three major evaluation cases:
     *
     * 1) Object types that cannot be evaluated by definition
     * 2) The object is a control method -- execute it
     * 3) The object is not a method -- just return it's current value
     */
    switch (AcpiNsGetType (Info->Node))
    {
    case ACPI_TYPE_DEVICE:
    case ACPI_TYPE_EVENT:
    case ACPI_TYPE_MUTEX:
    case ACPI_TYPE_REGION:
    case ACPI_TYPE_THERMAL:
    case ACPI_TYPE_LOCAL_SCOPE:
        /*
         * 1) Disallow evaluation of certain object types. For these,
         *    object evaluation is undefined and not supported.
         */
        ACPI_ERROR ((AE_INFO,
            "%s: Evaluation of object type [%s] is not supported",
            Info->FullPathname,
            AcpiUtGetTypeName (Info->Node->Type)));

        Status = AE_TYPE;
        goto Cleanup;

    case ACPI_TYPE_METHOD:
        /*
         * 2) Object is a control method - execute it
         */

        /* Verify that there is a method object associated with this node */

        if (!Info->ObjDesc)
        {
            ACPI_ERROR ((AE_INFO, "%s: Method has no attached sub-object",
                Info->FullPathname));
            Status = AE_NULL_OBJECT;
            goto Cleanup;
        }

        ACPI_DEBUG_PRINT ((ACPI_DB_EXEC,
            "**** Execute method [%s] at AML address %p length %X\n",
            Info->FullPathname,
            Info->ObjDesc->Method.AmlStart + 1,
            Info->ObjDesc->Method.AmlLength - 1));

        /*
         * Any namespace deletion must acquire both the namespace and
         * interpreter locks to ensure that no thread is using the portion of
         * the namespace that is being deleted.
         *
         * Execute the method via the interpreter. The interpreter is locked
         * here before calling into the AML parser
         */
        AcpiExEnterInterpreter ();
        Status = AcpiPsExecuteMethod (Info);
        AcpiExExitInterpreter ();
        break;

    default:
        /*
         * 3) All other non-method objects -- get the current object value
         */

        /*
         * Some objects require additional resolution steps (e.g., the Node
         * may be a field that must be read, etc.) -- we can't just grab
         * the object out of the node.
         *
         * Use ResolveNodeToValue() to get the associated value.
         *
         * NOTE: we can get away with passing in NULL for a walk state because
         * the Node is guaranteed to not be a reference to either a method
         * local or a method argument (because this interface is never called
         * from a running method.)
         *
         * Even though we do not directly invoke the interpreter for object
         * resolution, we must lock it because we could access an OpRegion.
         * The OpRegion access code assumes that the interpreter is locked.
         */
        AcpiExEnterInterpreter ();

        /* TBD: ResolveNodeToValue has a strange interface, fix */

        Info->ReturnObject = ACPI_CAST_PTR (ACPI_OPERAND_OBJECT, Info->Node);

        Status = AcpiExResolveNodeToValue (ACPI_CAST_INDIRECT_PTR (
            ACPI_NAMESPACE_NODE, &Info->ReturnObject), NULL);
        AcpiExExitInterpreter ();

        if (ACPI_FAILURE (Status))
        {
            Info->ReturnObject = NULL;
            goto Cleanup;
        }

        ACPI_DEBUG_PRINT ((ACPI_DB_NAMES, "Returned object %p [%s]\n",
            Info->ReturnObject,
            AcpiUtGetObjectTypeName (Info->ReturnObject)));

        Status = AE_CTRL_RETURN_VALUE; /* Always has a "return value" */
        break;
    }

    /*
     * For predefined names, check the return value against the ACPI
     * specification. Some incorrect return value types are repaired.
     */
    (void) AcpiNsCheckReturnValue (Info->Node, Info, Info->ParamCount,
        Status, &Info->ReturnObject);

    /* Check if there is a return value that must be dealt with */

    if (Status == AE_CTRL_RETURN_VALUE)
    {
        /* If caller does not want the return value, delete it */

        if (Info->Flags & ACPI_IGNORE_RETURN_VALUE)
        {
            AcpiUtRemoveReference (Info->ReturnObject);
            Info->ReturnObject = NULL;
        }

        /* Map AE_CTRL_RETURN_VALUE to AE_OK, we are done with it */
 
         Status = AE_OK;
     }
    else if (ACPI_FAILURE(Status)) 
    {
        /* If ReturnObject exists, delete it */

        if (Info->ReturnObject) 
        {
            AcpiUtRemoveReference (Info->ReturnObject);
            Info->ReturnObject = NULL;
        }
    }
 
     ACPI_DEBUG_PRINT ((ACPI_DB_NAMES,
         "*** Completed evaluation of object %s ***\n",
        Info->RelativePathname));

Cleanup:
    /*
     * Namespace was unlocked by the handling AcpiNs* function, so we
     * just free the pathname and return
     */
    ACPI_FREE (Info->FullPathname);
    Info->FullPathname = NULL;
    return_ACPI_STATUS (Status);
}
