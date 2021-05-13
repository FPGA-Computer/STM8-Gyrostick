// C:\Projects\Projects\STM8\USB_Motion\Descriptor Tool V2.4\6axis joystick 2reports-2.h


char ReportDescriptor[49] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    //   COLLECTION (Application)
    0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    //   USAGE (Pointer)
    0x16, 0x00, 0x80,              //   LOGICAL_MINIMUM (-32768)
    0x26, 0xff, 0x7f,              //   LOGICAL_MAXIMUM (32767)
    0x75, 0x10,                    //   REPORT_SIZE (16)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x85, 0x01,                    //     REPORT_ID (1)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x32,                    //     USAGE (Z)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //       END_COLLECTION
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x85, 0x02,                    //     REPORT_ID (2)
    0x09, 0x33,                    //     USAGE (Rx)
    0x09, 0x34,                    //     USAGE (Ry)
    0x09, 0x35,                    //     USAGE (Rz)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //       END_COLLECTION
    0xc0                           //   END_COLLECTION
};

