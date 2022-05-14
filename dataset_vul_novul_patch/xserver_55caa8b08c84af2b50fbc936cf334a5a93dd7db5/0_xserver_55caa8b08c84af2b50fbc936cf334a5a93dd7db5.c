ProcXFixesSetCursorName(ClientPtr client)
{
    CursorPtr pCursor;
    char *tchar;

    REQUEST(xXFixesSetCursorNameReq);
     REQUEST(xXFixesSetCursorNameReq);
     Atom atom;
 
    REQUEST_FIXED_SIZE(xXFixesSetCursorNameReq, stuff->nbytes);
     VERIFY_CURSOR(pCursor, stuff->cursor, client, DixSetAttrAccess);
     tchar = (char *) &stuff[1];
     atom = MakeAtom(tchar, stuff->nbytes, TRUE);
        return BadAlloc;

    pCursor->name = atom;
    return Success;
}
