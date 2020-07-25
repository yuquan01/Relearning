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
@interface SuperObject : NSObject
@end
@implementation SuperObject
@end

@interface SubObject : SuperObject
@end
@implementation SubObject
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        //NSObject
        //SuperObject : NSObject
        //SubObject : SuperObject
        
        NSObject *obj = [[NSObject alloc] init];
        SuperObject *superObj = [[SuperObject alloc] init];
        SubObject *subObj = [[SubObject alloc] init];
        
        BOOL a = [superObj isKindOfClass:[NSObject class]];
        
        BOOL b = [subObj isKindOfClass:[NSObject class]];
        
        
        BOOL c = [superObj isMemberOfClass:[NSObject class]];
        BOOL d = [subObj isMemberOfClass:[NSObject class]];
        
        
//        NSObject *obj1 = [obj init];
    }
    return 0;
}
