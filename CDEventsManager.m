/**
 * CDEvents
 *
 * Copyright (c) 2010-2013 Aron Cedercrantz
 * http://github.com/rastersize/CDEvents/
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#import "CDEventsManager.h"
#import "CDEventsManagerDelegate.h"


#define MD_DEBUG 1

#if MD_DEBUG
#define MDLog(...) NSLog(__VA_ARGS__)
#else
#define MDLog(...)
#endif

#ifndef __has_feature
	#define __has_feature(x) 0
#endif

#if !__has_feature(objc_arc)
	#error CDEvents must be built with ARC.
#endif

#if !__has_feature(blocks)
	#error CDEvents must be built with support for blocks.
#endif


#pragma mark CDEvents custom exceptions
NSString *const CDEventsEventStreamCreationFailureException = @"CDEventsEventStreamCreationFailureException";

#pragma -
#pragma mark Default values
const CDEventsEventStreamCreationFlags kCDEventsDefaultEventStreamFlags =
	(kFSEventStreamCreateFlagUseCFTypes |
	 kFSEventStreamCreateFlagWatchRoot);

const CDEventIdentifier kCDEventsSinceEventNow = kFSEventStreamEventIdSinceNow;

const NSTimeInterval kCDEventsDefaultNotificationLatency = 3.0;

const BOOL kCDEventsDefaultIgnoreEventFromSubDirs = NO;

#pragma mark -
#pragma mark Private API
// Private API
@interface CDEventsManager () {
@private
	CDEventsEventBlock                          _eventBlock;
	
	FSEventStreamRef							_eventStream;
	CDEventsEventStreamCreationFlags			_eventStreamCreationFlags;
}

// Redefine the properties that should be writeable.
@property (strong, readwrite) CDEvent *lastEvent;
@property (copy, readwrite) NSArray<NSURL *> *watchedURLs;

// The FSEvents callback function
static void CDEventsCallback(
	ConstFSEventStreamRef streamRef,
	void *callbackCtxInfo,
	size_t numEvents,
	void *eventPaths,
	const FSEventStreamEventFlags eventFlags[],
	const FSEventStreamEventId eventIds[]);

// Creates and initiates the event stream.
- (void)createEventStream;
// Disposes of the event stream.
- (void)disposeEventStream;

@end


#pragma mark -
#pragma mark Implementation
@implementation CDEventsManager

#pragma mark Properties
@synthesize delegate						= _delegate;
@synthesize notificationLatency				= _notificationLatency;
@synthesize sinceEventIdentifier			= _sinceEventIdentifier;
@synthesize ignoreEventsFromSubDirectories	= _ignoreEventsFromSubDirectories;
@synthesize lastEvent						= _lastEvent;
@synthesize watchedURLs						= _watchedURLs;
@synthesize excludedURLs					= _excludedURLs;


#pragma mark Event identifier class methods
+ (CDEventIdentifier)currentEventIdentifier {
	return (NSUInteger)FSEventsGetCurrentEventId();
}


#pragma mark Init/dealloc/finalize methods
- (void)dealloc {
	MDLog(@"[%@ %@]", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
	[self disposeEventStream];
	
	_delegate = nil;
}

//- (void)finalize
//{
//	[self disposeEventStream];
//	
//	_delegate = nil;
//	
//	[super finalize];
//}

- (instancetype)init {
	return [self initWithURLs:nil delegate:nil];
}

- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs delegate:(id<CDEventsManagerDelegate>)delegate {
	return [self initWithURLs:URLs
					 delegate:delegate
					onRunLoop:[NSRunLoop currentRunLoop]];
}

- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs
					delegate:(id<CDEventsManagerDelegate>)delegate
				   onRunLoop:(NSRunLoop *)runLoop {
	
	return [self initWithURLs:URLs
					 delegate:delegate
					onRunLoop:runLoop
		 sinceEventIdentifier:kCDEventsSinceEventNow
		 notificationLantency:CD_EVENTS_DEFAULT_NOTIFICATION_LATENCY
	  ignoreEventsFromSubDirs:CD_EVENTS_DEFAULT_IGNORE_EVENT_FROM_SUB_DIRS
				  excludeURLs:nil
		  streamCreationFlags:kCDEventsDefaultEventStreamFlags];
}

- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs
					delegate:(id<CDEventsManagerDelegate>)delegate
				   onRunLoop:(NSRunLoop *)runLoop
		sinceEventIdentifier:(CDEventIdentifier)sinceEventIdentifier
		notificationLantency:(CFTimeInterval)notificationLatency
	 ignoreEventsFromSubDirs:(BOOL)ignoreEventsFromSubDirs
				 excludeURLs:(NSArray<NSURL *> *)exludeURLs
		 streamCreationFlags:(CDEventsEventStreamCreationFlags)streamCreationFlags {
	
	if (delegate == nil) {
		[NSException raise:NSInvalidArgumentException format:@"Invalid arguments passed to CDEvents init-method."];
	}
	
	MDLog(@"[%@ %@]", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
	
	_delegate = delegate;
	
	return [self initWithURLs:URLs
						block:^(CDEventsManager *watcher, CDEvent *event){
//							MDLog(@"[%@ %@]", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
							
							if ([(id)[watcher delegate] conformsToProtocol:@protocol(CDEventsManagerDelegate)]) {
								[[watcher delegate] eventsManager:watcher eventOccurred:event];
							}
						}
					onRunLoop:runLoop
		 sinceEventIdentifier:sinceEventIdentifier
		 notificationLantency:notificationLatency
	  ignoreEventsFromSubDirs:ignoreEventsFromSubDirs
				  excludeURLs:exludeURLs
		  streamCreationFlags:streamCreationFlags];
}


#pragma mark Creating CDEvents Objects With a Block
- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs block:(CDEventsEventBlock)block {
	return [self initWithURLs:URLs block:block onRunLoop:[NSRunLoop currentRunLoop]];
}

- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs
					   block:(CDEventsEventBlock)block
				   onRunLoop:(NSRunLoop *)runLoop {
	return [self initWithURLs:URLs
						block:block
					onRunLoop:runLoop
		 sinceEventIdentifier:kCDEventsSinceEventNow
		 notificationLantency:CD_EVENTS_DEFAULT_NOTIFICATION_LATENCY
	  ignoreEventsFromSubDirs:CD_EVENTS_DEFAULT_IGNORE_EVENT_FROM_SUB_DIRS
				  excludeURLs:nil
		  streamCreationFlags:kCDEventsDefaultEventStreamFlags];
}

- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs
					   block:(CDEventsEventBlock)block
				   onRunLoop:(NSRunLoop *)runLoop
		sinceEventIdentifier:(CDEventIdentifier)sinceEventIdentifier
		notificationLantency:(CFTimeInterval)notificationLatency
	 ignoreEventsFromSubDirs:(BOOL)ignoreEventsFromSubDirs
				 excludeURLs:(nullable NSArray<NSURL *> *)exludeURLs
		 streamCreationFlags:(CDEventsEventStreamCreationFlags)streamCreationFlags {
	
	if (block == NULL || URLs == nil || [URLs count] == 0) {
		[NSException raise:NSInvalidArgumentException format:@"Invalid arguments passed to CDEvents init-method."];
	}
	
	if ((self = [super init])) {
		MDLog(@"[%@ %@]", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
		
		_watchedURLs = [URLs copy];
		_excludedURLs = [exludeURLs copy];
		_eventBlock = block;
		
		_sinceEventIdentifier = sinceEventIdentifier;
		_eventStreamCreationFlags = streamCreationFlags;
		
		_notificationLatency = notificationLatency;
		_ignoreEventsFromSubDirectories = ignoreEventsFromSubDirs;
		
		_lastEvent = nil;
		
		[self createEventStream];
		
		FSEventStreamScheduleWithRunLoop(_eventStream,
										 [runLoop getCFRunLoop],
										 kCFRunLoopDefaultMode);
		if (!FSEventStreamStart(_eventStream)) {
			[NSException raise:CDEventsEventStreamCreationFailureException
						format:@"Failed to create event stream."];
		}
	}
	
	return self;
}


#pragma mark NSCopying method
- (id)copyWithZone:(NSZone *)zone
{
	CDEventsManager *copy = [[CDEventsManager alloc] initWithURLs:[self watchedURLs]
											  block:[self eventBlock]
										  onRunLoop:[NSRunLoop currentRunLoop]
							   sinceEventIdentifier:[self sinceEventIdentifier]
							   notificationLantency:[self notificationLatency]
							ignoreEventsFromSubDirs:[self ignoreEventsFromSubDirectories]
										excludeURLs:[self excludedURLs]
								streamCreationFlags:_eventStreamCreationFlags];
	
	return copy;
}

#pragma mark Block
- (CDEventsEventBlock)eventBlock
{
	return _eventBlock;
}


#pragma mark Flush methods
- (void)flushSynchronously
{
	FSEventStreamFlushSync(_eventStream);
}

- (void)flushAsynchronously
{
	FSEventStreamFlushAsync(_eventStream);
}


#pragma mark Misc methods
- (NSString *)description {
	NSMutableString *description = [NSMutableString stringWithFormat:@"<%@: %p> ", NSStringFromClass(self.class), self];
	[description appendFormat:@", watchedURLs == {\n"];
	for (NSURL *watchedURL in _watchedURLs) {
		[description appendFormat:@"       \"%@\"\n", watchedURL.path];
	}
	[description appendFormat:@"}\n"];
	return description;
}

- (NSString *)streamDescription
{
	CFStringRef streamDescriptionCF = FSEventStreamCopyDescription(_eventStream);
	NSString *returnString = [[NSString alloc] initWithString:(__bridge NSString *)streamDescriptionCF];
	CFRelease(streamDescriptionCF);
	
	return returnString;
}


#pragma mark Private API:
- (void)createEventStream
{
	FSEventStreamContext callbackCtx;
	callbackCtx.version			= 0;
	callbackCtx.info			= (__bridge void *)self;
	callbackCtx.retain			= NULL;
	callbackCtx.release			= NULL;
	callbackCtx.copyDescription	= NULL;
	
	NSMutableArray *watchedPaths = [NSMutableArray arrayWithCapacity:[[self watchedURLs] count]];
	for (NSURL *URL in [self watchedURLs]) {
		[watchedPaths addObject:[URL path]];
	}
	
	_eventStream = FSEventStreamCreate(kCFAllocatorDefault,
									   &CDEventsCallback,
									   &callbackCtx,
									   (__bridge CFArrayRef)watchedPaths,
									   (FSEventStreamEventId)[self sinceEventIdentifier],
									   [self notificationLatency],
									   (uint) _eventStreamCreationFlags);
}

- (void)disposeEventStream
{
	if (!(_eventStream)) {
		return;
	}
	
	FSEventStreamStop(_eventStream);
	FSEventStreamInvalidate(_eventStream);
	FSEventStreamRelease(_eventStream);
	_eventStream = NULL;
}

static void CDEventsCallback(
	ConstFSEventStreamRef streamRef,
	void *callbackCtxInfo,
	size_t numEvents,
	void *eventPaths, // CFArrayRef
	const FSEventStreamEventFlags eventFlags[],
	const FSEventStreamEventId eventIds[])
{
	CDEventsManager *eventsManager			= (__bridge CDEventsManager *)callbackCtxInfo;
	NSArray *eventPathsArray	= (__bridge NSArray *)eventPaths;
	NSArray *watchedURLs		= [eventsManager watchedURLs];
	NSArray *excludedURLs		= [eventsManager excludedURLs];
	CDEvent *lastEvent			= nil;

//	NSLog(@"HERE");
	
	for (NSUInteger i = 0; i < numEvents; ++i) {
		BOOL shouldIgnore = NO;
		FSEventStreamEventFlags flags = eventFlags[i];
		FSEventStreamEventId identifier = eventIds[i];
		
		// We do this hackery to ensure that the eventPath string doesn't
		// contain any trailing slash.
		NSString *eventPath = [[eventPathsArray objectAtIndex:i] stringByStandardizingPath];
		NSString *eventParentDirPath = [eventPath stringByDeletingLastPathComponent];
		
		if ([eventsManager ignoreEventsFromSubDirectories]) {
			shouldIgnore = YES;
			for (NSURL *watchedURL in watchedURLs) {
				if ([watchedURL.path isEqualToString:eventParentDirPath]) {
					shouldIgnore = NO;
					break;
				}
			}
			
		// Ignore all explicitly excludeded URLs (not required to check if we
		// ignore all events from sub-directories).
		} else if (excludedURLs != nil) {
			for (NSURL *url in excludedURLs) {
				if ([eventPath hasPrefix:[url path]]) {
					shouldIgnore = YES;
					break;
				}
			}
		}
		
		if (!shouldIgnore) {
			CDEvent *event = [[CDEvent alloc] initWithIdentifier:identifier date:[NSDate date] URL:[NSURL fileURLWithPath:eventPath] flags:flags];
			lastEvent = event;
			
			CDEventsEventBlock eventBlock = [eventsManager eventBlock];
			eventBlock(eventsManager, event);
		}
	}
	
	if (lastEvent) {
		[eventsManager setLastEvent:lastEvent];
	}
}

@end
