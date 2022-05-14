 dwarf_elf_object_access_load_section(void* obj_in,
     Dwarf_Half section_index,
    Dwarf_Small** section_data,
    int* error)
{
    dwarf_elf_object_access_internals_t*obj =
        (dwarf_elf_object_access_internals_t*)obj_in;
    if (section_index == 0) {
        return DW_DLV_NO_ENTRY;
    }

    {
        Elf_Scn *scn = 0;
        Elf_Data *data = 0;

        scn = elf_getscn(obj->elf, section_index);
        if (scn == NULL) {
            *error = DW_DLE_MDE;
            return DW_DLV_ERROR;
        }

        /*  When using libelf as a producer, section data may be stored
            in multiple buffers. In libdwarf however, we only use libelf
            as a consumer (there is a dwarf producer API, but it doesn't
            use libelf). Because of this, this single call to elf_getdata
            will retrieve the entire section in a single contiguous
            buffer. */
        data = elf_getdata(scn, NULL);
        if (data == NULL) {
             *error = DW_DLE_MDE;
             return DW_DLV_ERROR;
         }
        if (!data->d_buf) {
            /*  If NULL it means 'the section has no data'
                according to libelf documentation.
                No DWARF-related section should ever have
                'no data'.  Happens if a section type is
                SHT_NOBITS and no section libdwarf
                wants to look at should be SHT_NOBITS. */
            *error = DW_DLE_MDE;
            return DW_DLV_ERROR;
        }
         *section_data = data->d_buf;
     }
     return DW_DLV_OK;
}
