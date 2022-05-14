AcpiPsCompleteFinalOp (
    ACPI_WALK_STATE         *WalkState,
     ACPI_PARSE_OBJECT       *Op,
     ACPI_STATUS             Status)
 {
    ACPI_STATUS             Status2;
 
 
     ACPI_FUNCTION_TRACE_PTR (PsCompleteFinalOp, WalkState);


    /*
     * Complete the last Op (if not completed), and clear the scope stack.
     * It is easily possible to end an AML "package" with an unbounded number
     * of open scopes (such as when several ASL blocks are closed with
     * sequential closing braces). We want to terminate each one cleanly.
     */
    ACPI_DEBUG_PRINT ((ACPI_DB_PARSE, "AML package complete at Op %p\n", Op));
    do
     {
         if (Op)
         {
            if (WalkState->AscendingCallback != NULL)
             {
                 WalkState->Op = Op;
                 WalkState->OpInfo = AcpiPsGetOpcodeInfo (Op->Common.AmlOpcode);
                WalkState->Opcode = Op->Common.AmlOpcode;

                Status = WalkState->AscendingCallback (WalkState);
                Status = AcpiPsNextParseState (WalkState, Op, Status);
                if (Status == AE_CTRL_PENDING)
                {
                    Status = AcpiPsCompleteOp (WalkState, &Op, AE_OK);
                    if (ACPI_FAILURE (Status))
                    {
                        return_ACPI_STATUS (Status);
                    }
                }
 
                 if (Status == AE_CTRL_TERMINATE)
                 {
                    Status = AE_OK;
                    /* Clean up */
                    do
                    {
                        if (Op)
                        {
                            Status2 = AcpiPsCompleteThisOp (WalkState, Op);
                            if (ACPI_FAILURE (Status2))
                            {
                                return_ACPI_STATUS (Status2);
                            }
                        }
                        AcpiPsPopScope (&(WalkState->ParserState), &Op,
                            &WalkState->ArgTypes, &WalkState->ArgCount);
                    } while (Op);
                    return_ACPI_STATUS (Status);
                 }
 
                 else if (ACPI_FAILURE (Status))
                 {
                     /* First error is most important */
 
                    (void) AcpiPsCompleteThisOp (WalkState, Op);
                    return_ACPI_STATUS (Status);
                 }
             }
 
            Status2 = AcpiPsCompleteThisOp (WalkState, Op);
            if (ACPI_FAILURE (Status2))
             {
                return_ACPI_STATUS (Status2);
             }
         }
 
        AcpiPsPopScope (&(WalkState->ParserState), &Op, &WalkState->ArgTypes,
            &WalkState->ArgCount);
 
     } while (Op);
 
    return_ACPI_STATUS (Status);
 }
