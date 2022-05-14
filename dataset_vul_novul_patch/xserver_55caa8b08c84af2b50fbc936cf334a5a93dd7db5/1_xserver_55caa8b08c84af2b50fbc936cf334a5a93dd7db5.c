ProcXFixesSetCursorName(ClientPtr client)
{
    CursorPtr pCursor;
    char *tchar;

    REQUEST(xXFixesSetCursorNameReq);
     REQUEST(xXFixesSetCursorNameReq);
     Atom atom;
 
    REQUEST_AT_LEAST_SIZE(xXFixesSetCursorNameReq);
     VERIFY_CURSOR(pCursor, stuff->cursor, client, DixSetAttrAccess);
     tchar = (char *) &stuff[1];
     atom = MakeAtom(tchar, stuff->nbytes, TRUE);
        return BadAlloc;

    pCursor->name = atom;
    return Success;
}
