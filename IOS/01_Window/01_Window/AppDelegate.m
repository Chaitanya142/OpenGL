//
//  AppDelegate.m
//  Window
//
//  Created by Apple on 28/01/20.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "MyView.h"

@implementation AppDelegate
{
    
@private
    UIWindow *mainWindow;
    ViewController *mainViewController;
    MyView *myView;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    //get screen bounds for full screen
    CGRect screenBounds = [[UIScreen mainScreen]bounds];
    
    //initialize window variable corresponding to screen bounds
    mainWindow = [[UIWindow alloc]initWithFrame:screenBounds];
    
    mainViewController = [[ViewController alloc]init];
    
    [mainWindow setRootViewController: mainViewController];
    
    //initialize view variable corresponding to screen bound
    myView = [[MyView alloc]initWithFrame:screenBounds];
    
    //Add the ViewController's view as subview to the window
    [mainViewController setView:myView];
    
    [myView release];
    
    [mainWindow addSubview:[mainViewController view]];
    
    //make window key window and visible
    [mainWindow makeKeyAndVisible];
    
    // Override point for customization after application launch.
    return YES;
}

-(void)applicationWillResignActive:(UIApplication *)application
{
    
}

-(void)applicationDidEnterBackground:(UIApplication *)application
{
    
}

-(void)applicationWillEnterForeground:(UIApplication *)application
{
    
}

-(void)applicationDidBecomeActive:(UIApplication *)application
{
    
}

-(void)applicationWillTerminate:(UIApplication *)application
{
    
}

-(void)dealloc
{
    //code
    [myView release];
    
    [mainViewController release];
    
    [mainWindow release];
    
    [super dealloc];
}
@end


