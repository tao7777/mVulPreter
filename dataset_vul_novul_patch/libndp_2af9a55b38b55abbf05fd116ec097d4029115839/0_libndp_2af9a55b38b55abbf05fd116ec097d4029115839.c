static bool ndp_msg_check_valid(struct ndp_msg *msg)
{
	size_t len = ndp_msg_payload_len(msg);
	enum ndp_msg_type msg_type = ndp_msg_type(msg);
 
 	if (len < ndp_msg_type_info(msg_type)->raw_struct_size)
 		return false;

	if (ndp_msg_type_info(msg_type)->addrto_validate)
		return ndp_msg_type_info(msg_type)->addrto_validate(&msg->addrto);
	else
		return true;
 }
