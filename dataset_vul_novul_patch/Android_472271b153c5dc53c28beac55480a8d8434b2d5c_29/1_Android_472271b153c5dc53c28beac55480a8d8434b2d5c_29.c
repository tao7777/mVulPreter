int uinput_create(char *name)
{
 struct uinput_dev dev;
 int fd, x = 0;

 
     for(x=0; x < MAX_UINPUT_PATHS; x++)
     {
        fd = open(uinput_dev_path[x], O_RDWR);
         if (fd < 0)
             continue;
         break;
 }
 if (x == MAX_UINPUT_PATHS) {
        BTIF_TRACE_ERROR("%s ERROR: uinput device open failed", __FUNCTION__);
 return -1;
 }
    memset(&dev, 0, sizeof(dev));
 if (name)
        strncpy(dev.name, name, UINPUT_MAX_NAME_SIZE-1);

    dev.id.bustype = BUS_BLUETOOTH;
    dev.id.vendor  = 0x0000;

     dev.id.product = 0x0000;
     dev.id.version = 0x0000;
 
    if (write(fd, &dev, sizeof(dev)) < 0) {
         BTIF_TRACE_ERROR("%s Unable to write device information", __FUNCTION__);
         close(fd);
         return -1;
     }
 
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);
 
     for (x = 0; key_map[x].name != NULL; x++)
        ioctl(fd, UI_SET_KEYBIT, key_map[x].mapped_id);
 
    if (ioctl(fd, UI_DEV_CREATE, NULL) < 0) {
         BTIF_TRACE_ERROR("%s Unable to create uinput device", __FUNCTION__);
         close(fd);
         return -1;
 }
 return fd;
}
