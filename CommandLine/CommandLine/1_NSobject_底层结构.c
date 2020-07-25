#pragma mark  ------------------------NSObject

//在开发中,几乎所有的类都继承自NSObject根类

#pragma mark  ------------------------使用clang将OC代码转化为C/C++代码
//基础命令
clang -rewrite-objc xxx.m
//可选参数 :
//指定xcode进行编译
xcrun
//指定SDK
-sdk iphoneos
-sdk iphonesimulator
//指定指令架构集
-arch arm64
//指定arc模式
-fobjc-arc
//指定runtime版本
-fobjc-runtime=ios-8.0.0
//指定输出路径
-o xxx.cpp
//指定依赖framework
-framework UIKit

//可能的形式为:
xcrun -sdk iphoneos clang -arch arm64 -rewrite-objc -fobjc-arc -fobjc-runtime=ios-8.0.0 xxx.m -o xxx.cpp -framework UIKit

#pragma mark  ------------------------NSObject具体实现

//NSOBject的定义,从中可以看出,其内部只有一个Class类型的isa成员
@interface NSObject <NSObject> {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-interface-ivars"
    Class isa  OBJC_ISA_AVAILABILITY;
#pragma clang diagnostic pop
}
//将代码转换成C/C++代码之后,其结构式一个结构体,内部也是只有一个Class类型的isa成员
struct NSObject_IMPL {
    Class isa;
};

//Class是一个struct objc_class *指针类型,也就是指向一个objc_class结构体的指针
typedef struct objc_class *Class;

//结构体objc_class的定义
//objc_class继承自objc_object
//objc_object结构体中只有一个成员,isa_t类型的成员isa
//objc_class结构体中有三个成员,
//    0、isa_t类型的isa,从objc_object继承而来
//    a、Class类型的superclass,可以看出其指向一个对象
//    b、cache_t类型的cache,这个字段后续解析,其内容是对曾经调用过的方法的集合,可以加快方法的查找
//    c、class_data_bits_t类型的bits,这个字段后续解析,其内容是对象的一些其他属性,如属性集合、方法集合、协议集合等
//这里只考虑objc2.0,在objc2.0之前的定义有所区别
struct objc_object {
    isa_t isa;
}
struct objc_class : objc_object {
    // Class ISA;
    Class superclass;
    cache_t cache;
    class_data_bits_t bits;
}

#pragma mark  ------------------------id
//id的定义,是一个struct objc_object *指针类型,也就是指向objc_object结构体的指针
//因此id可以用来表示任何对象,因为任何对象都是继承自objc_object,也就是都有isa_t 类型的isa
typedef struct objc_object *id;

#pragma mark  ------------------------id、Class、NSObject *、instance 的区别
/*
从其的数据结构体可以看出
id : 能表示包括但不限于NSObject对象等类型数据,只要其底层结构满足objc_object结构类型,如NSProxy、Block等
Class : 只能表示NSObject对象,因为只有NSObject才满足objc_class结构类型
NSObject * : 指向NSObject数据类型的指针
instance : 一般作为返回值,表示与当前类相同类型的对象
*/

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

#pragma mark  ------------------------总结
/*
 1、在OC中,几乎所有的类都是继承自NSObject
 2、在底层中,NSObject实际上是一个objc_class结构体,其中,objc_class结构体继承自objc_object结构体
 3、objc_object结构体内部只有一个isa_t类型的isa成员
 4、objc_class结构体中有三个成员,分别是superclass、cache、bits,当然,还包括一个继承得来的isa成员
 5、id、Class、NSObject *、instance 的区别
 6、自定义类的中成员变量都会在编译之后添加到对应的类结构体中
 */
