AcpiNsTerminate (
     void)
 {
     ACPI_STATUS             Status;
 
 
     ACPI_FUNCTION_TRACE (NsTerminate);
 
 
#ifdef ACPI_EXEC_APP
    {
        ACPI_OPERAND_OBJECT     *Prev;
        ACPI_OPERAND_OBJECT     *Next;
        /* Delete any module-level code blocks */
 
        Next = AcpiGbl_ModuleCodeList;
        while (Next)
        {
            Prev = Next;
            Next = Next->Method.Mutex;
            Prev->Method.Mutex = NULL; /* Clear the Mutex (cheated) field */
            AcpiUtRemoveReference (Prev);
        }
     }
#endif
 
     /*
      * Free the entire namespace -- all nodes and all objects
     * attached to the nodes
     */
    AcpiNsDeleteNamespaceSubtree (AcpiGbl_RootNode);

    /* Delete any objects attached to the root node */

    Status = AcpiUtAcquireMutex (ACPI_MTX_NAMESPACE);
    if (ACPI_FAILURE (Status))
    {
        return_VOID;
    }

    AcpiNsDeleteNode (AcpiGbl_RootNode);
    (void) AcpiUtReleaseMutex (ACPI_MTX_NAMESPACE);

    ACPI_DEBUG_PRINT ((ACPI_DB_INFO, "Namespace freed\n"));
    return_VOID;
}
