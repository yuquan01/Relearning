#pragma mark  ------------------------初识
//在objc_class结构体中,有一个成员是superclass,类型是Class
struct objc_class : objc_object {
    // Class ISA;
    Class superclass;
    cache_t cache;
    class_data_bits_t bits;
}

#pragma mark  ------------------------superclass
//从其数据类型可以看出,它指向一个Class数据,即指向一个对象
//从命名可以猜出,它指向当前对象的父类

#pragma mark  ------------------------总结
/*
 1、Class->superclass 指向一个对象,而且是当前对象的父对象
 */
