//
//  main.m
//  Window
//
//  Created by Apple on 28/01/20.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[]) {
    
    NSAutoreleasePool *pPool = [[NSAutoreleasePool alloc]init];
    
    NSString * appDelegateClassName;
    
    appDelegateClassName = NSStringFromClass([AppDelegate class]);
    
    int ret = UIApplicationMain(argc, argv, nil, appDelegateClassName);
    
    [pPool release];
    
    return(ret);
}

