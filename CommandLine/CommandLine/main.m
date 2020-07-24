//
//  main.m
//  CommandLine
//
//  Created by 肖郡郁 on 2020/7/22.
//  Copyright © 2020 x. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

#pragma mark -----
@interface YQ_Object : NSObject
//{
//    int age;
//    NSString *name;
//    NSMutableString *info;
//}
//    @property (nonatomic,assign) int weight;
//    @property (nonatomic,copy) NSString *nickName;
//    @property (nonatomic,strong) NSMutableString *addr;
@end
@implementation YQ_Object



@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        
        NSObject *obj1 = [YQ_Object alloc];
        NSObject *obj2 = [[YQ_Object alloc] init];
        Class class1 = [obj1 class];
        Class class2 = [YQ_Object class];
        Class class3 = NSClassFromString(@"YQ_Object");
        Class class4 = object_getClass(obj1);
        
        Class metaClass1 = object_getClass([YQ_Object class]);
        Class metaClass2 = objc_getMetaClass("YQ_Object");
        
//        NSObject *obj1 = [obj init];
    }
    return 0;
}
