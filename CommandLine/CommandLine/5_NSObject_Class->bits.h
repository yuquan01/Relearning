#pragma mark  ------------------------objc_class
//在objc_class结构体中,有一个成员是bits,类型是class_data_bits_t，从其注释可以看出，他的值是 class_rw_t * 与某个定义好的数据的进行某种操作之后的值
struct objc_class : objc_object {
    // Class ISA;
    Class superclass;
    cache_t cache;
    class_data_bits_t bits;// class_rw_t * plus custom rr/alloc flags
}

#pragma mark  ------------------------bits
//class_data_bits_t 的定义，其内部只有一个 uintptr_t 类型的 bits成员，依据之前的注释，可以找到class_rw_t *data数据，
//class_rw_t *data 数据是将 bits & FAST_DATA_MASK 之后得到的值
//因此，可以认为，class_data_bits_t实际上并没有存放什么实际上的数据，只是做了一个数据的中转，具体的内容存放在了class_rw_t数据结构中

struct class_data_bits_t {
    friend objc_class;

    // Values are the FAST_ flags above.
    uintptr_t bits;

    class_rw_t* data() const {
        return (class_rw_t *)(bits & FAST_DATA_MASK);
    }
    //other
};
//在64位系统中 FAST_DATA_MASK

#define FAST_DATA_MASK          0x00007ffffffffff8UL

#pragma mark  ------------------------class_rw_t
//class_rw_t 定义
struct class_rw_t {
    // Be warned that Symbolication knows the layout of this structure.
    uint32_t flags;
    uint16_t witness;
#if SUPPORT_INDEXED_ISA
    uint16_t index;
#endif

    explicit_atomic<uintptr_t> ro_or_rw_ext;

    Class firstSubclass;
    Class nextSiblingClass;
    //other
};






#pragma mark  ------------------------总结
