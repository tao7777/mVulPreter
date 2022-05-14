 static void usb_ehci_pci_exit(PCIDevice *dev)
 {
     EHCIPCIState *i = PCI_EHCI(dev);
static void usb_ehci_pci_reset(DeviceState *dev)
{
    PCIDevice *pci_dev = PCI_DEVICE(dev);
    EHCIPCIState *i = PCI_EHCI(pci_dev);
    EHCIState *s = &i->ehci;

    ehci_reset(s);
}

static void usb_ehci_pci_write_config(PCIDevice *dev, uint32_t addr,
                                      uint32_t val, int l)
{
    EHCIPCIState *i = PCI_EHCI(dev);
    bool busmaster;

    pci_default_write_config(dev, addr, val, l);

    if (!range_covers_byte(addr, l, PCI_COMMAND)) {
        return;
    }
    busmaster = pci_get_word(dev->config + PCI_COMMAND) & PCI_COMMAND_MASTER;
    i->ehci.as = busmaster ? pci_get_address_space(dev) : &address_space_memory;
}

static Property ehci_pci_properties[] = {
    DEFINE_PROP_UINT32("maxframes", EHCIPCIState, ehci.maxframes, 128),
    DEFINE_PROP_END_OF_LIST(),
};

static const VMStateDescription vmstate_ehci_pci = {
    .name        = "ehci",
    .version_id  = 2,
    .minimum_version_id  = 1,
    .fields = (VMStateField[]) {
        VMSTATE_PCI_DEVICE(pcidev, EHCIPCIState),
        VMSTATE_STRUCT(ehci, EHCIPCIState, 2, vmstate_ehci, EHCIState),
        VMSTATE_END_OF_LIST()
    }
};

static void ehci_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->realize = usb_ehci_pci_realize;
    k->exit = usb_ehci_pci_exit;
    k->class_id = PCI_CLASS_SERIAL_USB;
    k->config_write = usb_ehci_pci_write_config;
    dc->vmsd = &vmstate_ehci_pci;
    dc->props = ehci_pci_properties;
    dc->reset = usb_ehci_pci_reset;
}

static const TypeInfo ehci_pci_type_info = {
    .name = TYPE_PCI_EHCI,
    .parent = TYPE_PCI_DEVICE,
    .instance_size = sizeof(EHCIPCIState),
    .instance_init = usb_ehci_pci_init,
    .abstract = true,
    .class_init = ehci_class_init,
};

static void ehci_data_class_init(ObjectClass *klass, void *data)
     .parent = TYPE_PCI_DEVICE,
     .instance_size = sizeof(EHCIPCIState),
     .instance_init = usb_ehci_pci_init,
    .instance_finalize = usb_ehci_pci_finalize,
     .abstract = true,
     .class_init = ehci_class_init,
 };
