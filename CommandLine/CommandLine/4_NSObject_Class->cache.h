#pragma mark  ------------------------初识
//在objc_class结构体中,有一个成员是cache,类型是cache_t
struct objc_class : objc_object {
    // Class ISA;
    Class superclass;
    cache_t cache;
    class_data_bits_t bits;
}

#pragma mark  ------------------------cache
//cache_t 定义 :
struct cache_t {
    static bucket_t *emptyBuckets();
    
    struct bucket_t *buckets();
    mask_t mask();
    mask_t occupied();
    //其他变量及方法
};

//bucket_t 定义 :
struct bucket_t {
#if __arm64__
    explicit_atomic<uintptr_t> _imp;
    explicit_atomic<SEL> _sel;
#else
    explicit_atomic<SEL> _sel;
    explicit_atomic<uintptr_t> _imp;
#endif
    //其他变量及方法
};
//
#pragma mark  ------------------------总结
