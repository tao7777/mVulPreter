int modbus_reply(modbus_t *ctx, const uint8_t *req,
                 int req_length, modbus_mapping_t *mb_mapping)
{
    int offset;
    int slave;
    int function;
    uint16_t address;
    uint8_t rsp[MAX_MESSAGE_LENGTH];
    int rsp_length = 0;
    sft_t sft;

    if (ctx == NULL) {
        errno = EINVAL;
        return -1;
    }

    offset = ctx->backend->header_length;
    slave = req[offset - 1];
    function = req[offset];
    address = (req[offset + 1] << 8) + req[offset + 2];

    sft.slave = slave;
    sft.function = function;
    sft.t_id = ctx->backend->prepare_response_tid(req, &req_length);

    /* Data are flushed on illegal number of values errors. */
    switch (function) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS: {
        unsigned int is_input = (function == MODBUS_FC_READ_DISCRETE_INPUTS);
        int start_bits = is_input ? mb_mapping->start_input_bits : mb_mapping->start_bits;
        int nb_bits = is_input ? mb_mapping->nb_input_bits : mb_mapping->nb_bits;
        uint8_t *tab_bits = is_input ? mb_mapping->tab_input_bits : mb_mapping->tab_bits;
        const char * const name = is_input ? "read_input_bits" : "read_bits";
        int nb = (req[offset + 3] << 8) + req[offset + 4];
        /* The mapping can be shifted to reduce memory consumption and it
           doesn't always start at address zero. */
        int mapping_address = address - start_bits;

        if (nb < 1 || MODBUS_MAX_READ_BITS < nb) {
            rsp_length = response_exception(
                ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE, rsp, TRUE,
                "Illegal nb of values %d in %s (max %d)\n",
                nb, name, MODBUS_MAX_READ_BITS);
        } else if (mapping_address < 0 || (mapping_address + nb) > nb_bits) {
            rsp_length = response_exception(
                ctx, &sft,
                MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp, FALSE,
                "Illegal data address 0x%0X in %s\n",
                mapping_address < 0 ? address : address + nb, name);
        } else {
            rsp_length = ctx->backend->build_response_basis(&sft, rsp);
            rsp[rsp_length++] = (nb / 8) + ((nb % 8) ? 1 : 0);
            rsp_length = response_io_status(tab_bits, mapping_address, nb,
                                            rsp, rsp_length);
        }
    }
        break;
    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS: {
        unsigned int is_input = (function == MODBUS_FC_READ_INPUT_REGISTERS);
        int start_registers = is_input ? mb_mapping->start_input_registers : mb_mapping->start_registers;
        int nb_registers = is_input ? mb_mapping->nb_input_registers : mb_mapping->nb_registers;
        uint16_t *tab_registers = is_input ? mb_mapping->tab_input_registers : mb_mapping->tab_registers;
        const char * const name = is_input ? "read_input_registers" : "read_registers";
        int nb = (req[offset + 3] << 8) + req[offset + 4];
        /* The mapping can be shifted to reduce memory consumption and it
           doesn't always start at address zero. */
        int mapping_address = address - start_registers;

        if (nb < 1 || MODBUS_MAX_READ_REGISTERS < nb) {
            rsp_length = response_exception(
                ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE, rsp, TRUE,
                "Illegal nb of values %d in %s (max %d)\n",
                nb, name, MODBUS_MAX_READ_REGISTERS);
        } else if (mapping_address < 0 || (mapping_address + nb) > nb_registers) {
            rsp_length = response_exception(
                ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp, FALSE,
                "Illegal data address 0x%0X in %s\n",
                mapping_address < 0 ? address : address + nb, name);
        } else {
            int i;

            rsp_length = ctx->backend->build_response_basis(&sft, rsp);
            rsp[rsp_length++] = nb << 1;
            for (i = mapping_address; i < mapping_address + nb; i++) {
                rsp[rsp_length++] = tab_registers[i] >> 8;
                rsp[rsp_length++] = tab_registers[i] & 0xFF;
            }
        }
    }
        break;
    case MODBUS_FC_WRITE_SINGLE_COIL: {
        int mapping_address = address - mb_mapping->start_bits;

        if (mapping_address < 0 || mapping_address >= mb_mapping->nb_bits) {
            rsp_length = response_exception(
                ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp, FALSE,
                "Illegal data address 0x%0X in write_bit\n",
                address);
        } else {
            int data = (req[offset + 3] << 8) + req[offset + 4];

            if (data == 0xFF00 || data == 0x0) {
                mb_mapping->tab_bits[mapping_address] = data ? ON : OFF;
                memcpy(rsp, req, req_length);
                rsp_length = req_length;
            } else {
                rsp_length = response_exception(
                    ctx, &sft,
                    MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE, rsp, FALSE,
                    "Illegal data value 0x%0X in write_bit request at address %0X\n",
                    data, address);
            }
        }
    }
        break;
    case MODBUS_FC_WRITE_SINGLE_REGISTER: {
        int mapping_address = address - mb_mapping->start_registers;

        if (mapping_address < 0 || mapping_address >= mb_mapping->nb_registers) {
            rsp_length = response_exception(
                ctx, &sft,
                MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp, FALSE,
                "Illegal data address 0x%0X in write_register\n",
                address);
        } else {
            int data = (req[offset + 3] << 8) + req[offset + 4];

            mb_mapping->tab_registers[mapping_address] = data;
            memcpy(rsp, req, req_length);
            rsp_length = req_length;
        }
    }
         break;
     case MODBUS_FC_WRITE_MULTIPLE_COILS: {
         int nb = (req[offset + 3] << 8) + req[offset + 4];
        int nb_bits = req[offset + 5];
         int mapping_address = address - mb_mapping->start_bits;
 
        if (nb < 1 || MODBUS_MAX_WRITE_BITS < nb || nb_bits * 8 < nb) {
             /* May be the indication has been truncated on reading because of
              * invalid address (eg. nb is 0 but the request contains values to
              * write) so it's necessary to flush. */
            rsp_length = response_exception(
                ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE, rsp, TRUE,
                "Illegal number of values %d in write_bits (max %d)\n",
                nb, MODBUS_MAX_WRITE_BITS);
        } else if (mapping_address < 0 ||
                   (mapping_address + nb) > mb_mapping->nb_bits) {
            rsp_length = response_exception(
                ctx, &sft,
                MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp, FALSE,
                "Illegal data address 0x%0X in write_bits\n",
                mapping_address < 0 ? address : address + nb);
        } else {
            /* 6 = byte count */
            modbus_set_bits_from_bytes(mb_mapping->tab_bits, mapping_address, nb,
                                       &req[offset + 6]);

            rsp_length = ctx->backend->build_response_basis(&sft, rsp);
            /* 4 to copy the bit address (2) and the quantity of bits */
            memcpy(rsp + rsp_length, req + rsp_length, 4);
            rsp_length += 4;
        }
    }
         break;
     case MODBUS_FC_WRITE_MULTIPLE_REGISTERS: {
         int nb = (req[offset + 3] << 8) + req[offset + 4];
        int nb_bytes = req[offset + 5];
         int mapping_address = address - mb_mapping->start_registers;
 
        if (nb < 1 || MODBUS_MAX_WRITE_REGISTERS < nb || nb_bytes * 8 < nb) {
             rsp_length = response_exception(
                 ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE, rsp, TRUE,
                 "Illegal number of values %d in write_registers (max %d)\n",
                nb, MODBUS_MAX_WRITE_REGISTERS);
        } else if (mapping_address < 0 ||
                   (mapping_address + nb) > mb_mapping->nb_registers) {
            rsp_length = response_exception(
                ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp, FALSE,
                "Illegal data address 0x%0X in write_registers\n",
                mapping_address < 0 ? address : address + nb);
        } else {
            int i, j;
            for (i = mapping_address, j = 6; i < mapping_address + nb; i++, j += 2) {
                /* 6 and 7 = first value */
                mb_mapping->tab_registers[i] =
                    (req[offset + j] << 8) + req[offset + j + 1];
            }

            rsp_length = ctx->backend->build_response_basis(&sft, rsp);
            /* 4 to copy the address (2) and the no. of registers */
            memcpy(rsp + rsp_length, req + rsp_length, 4);
            rsp_length += 4;
        }
    }
        break;
    case MODBUS_FC_REPORT_SLAVE_ID: {
        int str_len;
        int byte_count_pos;

        rsp_length = ctx->backend->build_response_basis(&sft, rsp);
        /* Skip byte count for now */
        byte_count_pos = rsp_length++;
        rsp[rsp_length++] = _REPORT_SLAVE_ID;
        /* Run indicator status to ON */
        rsp[rsp_length++] = 0xFF;
        /* LMB + length of LIBMODBUS_VERSION_STRING */
        str_len = 3 + strlen(LIBMODBUS_VERSION_STRING);
        memcpy(rsp + rsp_length, "LMB" LIBMODBUS_VERSION_STRING, str_len);
        rsp_length += str_len;
        rsp[byte_count_pos] = rsp_length - byte_count_pos - 1;
    }
        break;
    case MODBUS_FC_READ_EXCEPTION_STATUS:
        if (ctx->debug) {
            fprintf(stderr, "FIXME Not implemented\n");
        }
        errno = ENOPROTOOPT;
        return -1;
        break;
    case MODBUS_FC_MASK_WRITE_REGISTER: {
        int mapping_address = address - mb_mapping->start_registers;

        if (mapping_address < 0 || mapping_address >= mb_mapping->nb_registers) {
            rsp_length = response_exception(
                ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp, FALSE,
                "Illegal data address 0x%0X in write_register\n",
                address);
        } else {
            uint16_t data = mb_mapping->tab_registers[mapping_address];
            uint16_t and = (req[offset + 3] << 8) + req[offset + 4];
            uint16_t or = (req[offset + 5] << 8) + req[offset + 6];

            data = (data & and) | (or & (~and));
            mb_mapping->tab_registers[mapping_address] = data;
            memcpy(rsp, req, req_length);
            rsp_length = req_length;
        }
    }
        break;
    case MODBUS_FC_WRITE_AND_READ_REGISTERS: {
        int nb = (req[offset + 3] << 8) + req[offset + 4];
        uint16_t address_write = (req[offset + 5] << 8) + req[offset + 6];
        int nb_write = (req[offset + 7] << 8) + req[offset + 8];
        int nb_write_bytes = req[offset + 9];
        int mapping_address = address - mb_mapping->start_registers;
        int mapping_address_write = address_write - mb_mapping->start_registers;

        if (nb_write < 1 || MODBUS_MAX_WR_WRITE_REGISTERS < nb_write ||
            nb < 1 || MODBUS_MAX_WR_READ_REGISTERS < nb ||
            nb_write_bytes != nb_write * 2) {
            rsp_length = response_exception(
                ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE, rsp, TRUE,
                "Illegal nb of values (W%d, R%d) in write_and_read_registers (max W%d, R%d)\n",
                nb_write, nb, MODBUS_MAX_WR_WRITE_REGISTERS, MODBUS_MAX_WR_READ_REGISTERS);
        } else if (mapping_address < 0 ||
                   (mapping_address + nb) > mb_mapping->nb_registers ||
                   mapping_address < 0 ||
                   (mapping_address_write + nb_write) > mb_mapping->nb_registers) {
            rsp_length = response_exception(
                ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp, FALSE,
                "Illegal data read address 0x%0X or write address 0x%0X write_and_read_registers\n",
                mapping_address < 0 ? address : address + nb,
                mapping_address_write < 0 ? address_write : address_write + nb_write);
        } else {
            int i, j;
            rsp_length = ctx->backend->build_response_basis(&sft, rsp);
            rsp[rsp_length++] = nb << 1;

            /* Write first.
               10 and 11 are the offset of the first values to write */
            for (i = mapping_address_write, j = 10;
                 i < mapping_address_write + nb_write; i++, j += 2) {
                mb_mapping->tab_registers[i] =
                    (req[offset + j] << 8) + req[offset + j + 1];
            }

            /* and read the data for the response */
            for (i = mapping_address; i < mapping_address + nb; i++) {
                rsp[rsp_length++] = mb_mapping->tab_registers[i] >> 8;
                rsp[rsp_length++] = mb_mapping->tab_registers[i] & 0xFF;
            }
        }
    }
        break;

    default:
        rsp_length = response_exception(
            ctx, &sft, MODBUS_EXCEPTION_ILLEGAL_FUNCTION, rsp, TRUE,
            "Unknown Modbus function code: 0x%0X\n", function);
        break;
    }

    /* Suppress any responses when the request was a broadcast */
    return (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU &&
            slave == MODBUS_BROADCAST_ADDRESS) ? 0 : send_msg(ctx, rsp, rsp_length);
}
