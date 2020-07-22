//目录下执行该命令,将oc文件转化为cpp文件,方便查看底层实现
xcrun -sdk iphoneos clang -arch arm64 -rewrite-objc main.m -o main.cpp

//底层的具体实现
struct objc_class {
    Class isa;
    
    Class super_class;
    const char * name;
    long version;
    long info;
    long instance_size;
    struct objc_ivar_list * ivars;
    struct objc_method_list * *  methodLists;
    struct objc_cache * cache;
    struct objc_protocol_list * protocols;
    
};
//class类型是一个objc_class类型的结构体指针别名,指向一个objc_class类型结构体的地址
typedef struct objc_class *Class;
//底层实现,定义的类实际上是一个结构体,内部有一个class类型的isa变量
struct NSObject_IMPL {
    Class isa;
};
//在OC内部,会将我们定义的NSObject的类转化为这种形式,里面会有一个class类型的isa变量
//此时还在OC语言层面
@interface NSObject {
    Class isa;
}
//在OC中定义的一个NSObject类
@interface NSObject {
}




