#pragma mark  ------------------------总结

//在objc_class结构体中,其中一个成员是class_data_bits_t类型的bits,主要用来保存一些对象的方法、属性、协议等信息

#pragma mark  ------------------------定义
//class_data_bits_t 结构,经过精简之后如下
//内部有一个成员bits,将 bits & FAST_DATA_MASK 操作后得到一个class_rw_t *指针类型数据
struct class_data_bits_t {
    uintptr_t bits;
    
    class_rw_t* data() const {
        return (class_rw_t *)(bits & FAST_DATA_MASK);
    }
    //还有一些其他方法
};
//class_rw_t 类型,经过精简之后如下
//从中可以看到主要保存了一些方法集合、属性集合、协议集合等信息
struct class_rw_t {
    uint32_t flags;
    uint16_t witness;
#if SUPPORT_INDEXED_ISA
    uint16_t index;
#endif
    
    const class_ro_t *ro;
    method_array_t methods;
    property_array_t properties;
    protocol_array_t protocols;
    char *demangledName;
    uint32_t version;
    
    Class firstSubclass;
    Class nextSiblingClass;
};
//class_ro_t 结构,经过精简之后如下
//从中可以看出,其内部也保存了一些方法集合、协议集合、属性集合等信息
struct class_ro_t {
    uint32_t flags;
    uint32_t instanceStart;
    uint32_t instanceSize;
#ifdef __LP64__
    uint32_t reserved;
#endif
    
    const uint8_t * ivarLayout;
    
    const char * name;
    method_list_t * baseMethodList;
    protocol_list_t * baseProtocols;
    const ivar_list_t * ivars;
    
    const uint8_t * weakIvarLayout;
    property_list_t *baseProperties;
}

#pragma mark  ------------------------总结
/*
 1、在Class->bits结构中,主要包含一个bits的成员,将其与一个特定的掩码经过与运算操作之后,将会得到一个class_rw_t *类型指针,
 2、class_rw_t是一个包含了一些方法集合、协议集合、属性集合、class_ro_t *类型数据ro等信息的一个结构体
 3、class_ro_t是一个包含了一些方法集合、协议集合、属性集合等信息的一个结构体
 4、其包含关系大致为 : Class->bits & FAST_DATA_MASK —> class_rw_t * -> class_ro_t *
 */

