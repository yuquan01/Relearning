#pragma mark  ------------------------NSObject初始

 //在开发中,几乎所有的类都继承自NSObject根类,那NSObject根类到底是什么呢?

#pragma mark  ------------------------NSObject具体实现

 //OC中的NSObject根类,其定义如下,内部有一个Class类型的isa成员变量
@interface NSObject <NSObject> {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-interface-ivars"
    Class isa  OBJC_ISA_AVAILABILITY;
#pragma clang diagnostic pop
}

//再看底层的具体实现,会发现是一个结构体,其内部有一个Class类型的isa成员变量
struct NSObject_IMPL {
    Class isa;
};

//而Class类型是什么呢,通过以下定义可以得知,Class类型是一个objc_class *的指针类型,指向一个objc_class类型的地址
typedef struct objc_class *Class;

//而对于objc_class,他是一个结构体,在不同的版本中其结构也是不一样的,这里只以objc2.0之后的版本进行分析
//objc_class是继承自objc_object结构体类型,因此objc_class结构体的第一个成员就是objc_object的成员
//可以看出,一个objc_class结构体内部有4个成员,其功能在后续会进行介绍
struct objc_object {
    isa_t isa;
}
struct objc_class : objc_object {
    // Class ISA;
    Class superclass;
    cache_t cache;
    class_data_bits_t bits;
}

#pragma mark  ------------------------自定义类

//在没有添加任何成员变量时,内部无变化
@interface YQ_Object : NSObject{
}
@end
struct YQ_Object_IMPL {
    struct NSObject_IMPL NSObject_IVARS;
};
//当添加了成员变量时,内部会在NSObject_IMPL结构体之后添加对应的成员变量到结构体中
@interface YQ_Object : NSObject{
    int age;
    NSString *name;
    NSMutableString *info;
}
@end

struct YQ_Object_IMPL {
    struct NSObject_IMPL NSObject_IVARS;
    int age;
    NSString *name;
    NSMutableString *info;
};






