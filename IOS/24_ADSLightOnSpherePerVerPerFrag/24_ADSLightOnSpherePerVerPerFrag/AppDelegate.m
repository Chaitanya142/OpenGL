//
//  AppDelegate.m
//  Window
//
//  Created by Apple on 28/01/20.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "GLESView.h"

@implementation AppDelegate
{
    
@private
    UIWindow *mainWindow;
    ViewController *mainViewController;
    GLESView *myView;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    //get screen bounds for full screen
    CGRect screenBounds = [[UIScreen mainScreen]bounds];
    
    //initialize window variable corresponding to screen bounds
    mainWindow = [[UIWindow alloc]initWithFrame:screenBounds];
    
    mainViewController = [[ViewController alloc]init];
    
    [mainWindow setRootViewController: mainViewController];
    
    //initialize view variable corresponding to screen bound
    myView = [[GLESView alloc]initWithFrame:screenBounds];
    
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
    [myView stopAnimation];
}

-(void)applicationDidEnterBackground:(UIApplication *)application
{
    
}

-(void)applicationWillEnterForeground:(UIApplication *)application
{
    
}

-(void)applicationDidBecomeActive:(UIApplication *)application
{
    [myView startAnimation];
}

-(void)applicationWillTerminate:(UIApplication *)application
{
    [myView stopAnimation];
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


