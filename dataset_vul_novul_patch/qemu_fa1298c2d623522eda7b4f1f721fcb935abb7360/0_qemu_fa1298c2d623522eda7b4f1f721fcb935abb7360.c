 static int ohci_bus_start(OHCIState *ohci)
 {
     trace_usb_ohci_start(ohci->name);
 
     /* Delay the first SOF event by one frame time as

    if (ohci->eof_timer == NULL) {
        trace_usb_ohci_bus_eof_timer_failed(ohci->name);
        ohci_die(ohci);
        return 0;
    }

    trace_usb_ohci_start(ohci->name);

    /* Delay the first SOF event by one frame time as
 static void ohci_bus_stop(OHCIState *ohci)
 {
     trace_usb_ohci_stop(ohci->name);
    timer_del(ohci->eof_timer);
 }
 
 /* Sets a flag in a port status register but only set it if the port is
}
