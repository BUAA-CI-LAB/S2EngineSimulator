#include "../../include/base/Object.hpp"

//MailBoxUser::counter = 0;

atomic<Object::PortNumType> Object::Port::portNum(0);

const array<unsigned Object::PortDataType,33>  Object::mask   ={
    /** 0**/0x0,
    /** 1**/0x1,
    /** 2**/0x3,
    /** 3**/0x7,
    /** 4**/0xf,
    /** 5**/0x1f,
    /** 6**/0x3f,
    /** 7**/0x7f,
    /** 8**/0xff,
    /** 9**/0x1ff,
    /**10**/0x3ff,
    /**11**/0x7ff,
    /**12**/0xfff,
    /**13**/0x1fff,
    /**14**/0x3fff,
    /**15**/0x7fff,
    /**16**/0xffff,
    /**17**/0x1ffff,
    /**18**/0x3ffff,
    /**19**/0x7ffff,
    /**20**/0xfffff,
    /**21**/0x1fffff,
    /**22**/0x3fffff,
    /**23**/0x7fffff,
    /**24**/0xffffff,
    /**25**/0x1ffffff,
    /**26**/0x3ffffff,
    /**27**/0x7ffffff,
    /**28**/0xfffffff,
    /**29**/0x1fffffff,
    /**30**/0x3fffffff,
    /**31**/0x7fffffff,
    /**32**/0xffffffff
};

const array<unsigned Object::PortDataType,33>  Object::extend ={
    /** 0**/0xffffffff,

    /** 1**/0xfffffffe,
    /** 2**/0xfffffffc,
    /** 3**/0xfffffff8,
    /** 4**/0xfffffff0,

    /** 5**/0xffffffe0,
    /** 6**/0xffffffc0,
    /** 7**/0xffffff80,
    /** 8**/0xffffff00,

    /** 9**/0xfffffe00,
    /**10**/0xfffffc00,
    /**11**/0xfffff800,
    /**12**/0xfffff000,

    /**13**/0xffffe000,
    /**14**/0xffffc000,
    /**15**/0xffff8000,
    /**16**/0xffff0000,

    /**17**/0xfffe0000,
    /**18**/0xfffc0000,
    /**19**/0xfff80000,
    /**20**/0xfff00000,

    /**21**/0xffe00000,
    /**22**/0xffc00000,
    /**23**/0xff800000,
    /**24**/0xff000000,

    /**25**/0xfe000000,
    /**26**/0xfc000000,
    /**27**/0xf8000000,
    /**28**/0xf0000000,

    /**29**/0xe0000000,
    /**30**/0xc0000000,
    /**31**/0x80000000,
    /**32**/0x00000000
};

const array<unsigned Object::PortDataType,33>  Object::highest={
    /** 0**/0x0,
    /** 1**/0x1,
    /** 2**/0x2,
    /** 3**/0x4,
    /** 4**/0x8,

    /** 5**/0x10,
    /** 6**/0x20,
    /** 7**/0x40,
    /** 8**/0x80,

    /** 9**/0x100,
    /**10**/0x200,
    /**11**/0x400,
    /**12**/0x800,

    /**13**/0x1000,
    /**14**/0x2000,
    /**15**/0x4000,
    /**16**/0x8000,

    /**17**/0x10000,
    /**18**/0x20000,
    /**19**/0x40000,
    /**20**/0x80000,

    /**21**/0x100000,
    /**22**/0x200000,
    /**23**/0x400000,
    /**24**/0x800000,

    /**25**/0x1000000,
    /**26**/0x2000000,
    /**27**/0x4000000,
    /**28**/0x8000000,

    /**29**/0x10000000,
    /**30**/0x20000000,
    /**31**/0x40000000,
    /**32**/0x80000000
};
