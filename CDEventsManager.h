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

/**
 * @headerfile CDEvents.h CDEvents/CDEventsManager.h
 * A class that wraps the <code>FSEvents</code> C API.
 * 
 * A class that wraps the <code>FSEvents</code> C API. Inspired and based
 * upon the open source project SCEvents created by Stuart Connolly
 * http://stuconnolly.com/projects/code/
 */

#import <Foundation/Foundation.h>
#import <CoreServices/CoreServices.h>

#import "CDEvent.h"

NS_ASSUME_NONNULL_BEGIN

@protocol CDEventsManagerDelegate;


#pragma mark -
#pragma mark CDEventsManager types
/**
 * The event stream creation flags type.
 *
 * @since 1.0.2
 */
typedef FSEventStreamCreateFlags CDEventsEventStreamCreationFlags;


#pragma mark -
#pragma mark CDEventsManager custom exceptions
/**
 * The exception raised if CDEventsManager failed to create the event stream.
 *
 * @since 1.0.0
 */
extern NSString *const CDEventsEventStreamCreationFailureException;


#pragma mark -
#pragma mark Default values
/**
 * The default notificaion latency.
 *
 * @since 1.0.0
 */
#define CD_EVENTS_DEFAULT_NOTIFICATION_LATENCY			((NSTimeInterval)3.0)

extern const NSTimeInterval kCDEventsDefaultNotificationLatency;

/**
 * The default value whether events from sub directories should be ignored or not.
 *
 * @since 1.0.0
 */
#define CD_EVENTS_DEFAULT_IGNORE_EVENT_FROM_SUB_DIRS	NO

extern const BOOL kCDEventsDefaultIgnoreEventFromSubDirs;

/**
 * The default event stream creation flags.
 *
 * @since 1.0.0
 */
extern const CDEventsEventStreamCreationFlags kCDEventsDefaultEventStreamFlags;

/**
 * Use this to get all event since now when initializing a CDEventsManager object.
 *
 * @since 1.1.0
 */
extern const CDEventIdentifier kCDEventsSinceEventNow;


#pragma mark -
#pragma mark CDEventsManager Block Type
@class CDEventsManager;
/**
 * Type of the block which gets called when an event occurs.
 *
 * @since head
 */
typedef void (^CDEventsEventBlock)(CDEventsManager *watcher, CDEvent *event);


#pragma mark -
#pragma mark CDEventsManager interface
/**
 * An Objective-C wrapper for the <code>FSEvents</code> C API.
 *
 * @note Inspired by <code>SCEvents</code> class of the <code>SCEvents</code> project by Stuart Connolly.
 *
 * @see FSEvents.h in CoreServices
 *
 * @since 1.0.0
 */
@interface CDEventsManager : NSObject <NSCopying> {}

#pragma mark Properties
/** @name Managing the Delegate */
/**
 * The delegate object the <code>CDEventsManager</code> object calls when it recieves an event.
 *
 * @param delegate Delegate for the events object. <code>nil</code> removes the delegate.
 * @return The events's delegate.
 *
 * @see CDEventsManagerDelegate
 *
 * @since 1.0.0
 */
@property (unsafe_unretained, nullable) id<CDEventsManagerDelegate>	delegate;

/** @name Getting Event Block */
/**
 * The event block.
 *
 * @return The CDEventsEventBlock block which is executed when an event occurs.
 *
 * @since head
 */
@property (readonly) CDEventsEventBlock				eventBlock;

/** @name Getting Event Watcher Properties */
/**
 * The (approximate) time intervall between notifications sent to the delegate.
 *
 * @return The time intervall between notifications.
 *
 * @since 1.0.0
 */
@property (readonly) CFTimeInterval					notificationLatency;

/**
 * The event identifier from which events will be supplied to the delegate.
 *
 * @return The event identifier from which events will be supplied to the delegate.
 *
 * @since 1.0.0
 */
@property (readonly) CDEventIdentifier				sinceEventIdentifier;

/**
 * The last event that occured and has been delivered to the delegate.
 *
 * @return The last event that occured and has been delivered to the delegate.
 *
 * @since 1.0.0
 */
@property (strong, readonly) CDEvent				*lastEvent;

/**
 * The URLs that we watch for events.
 *
 * @return An array of <code>NSURL</code> object for the URLs which we watch for events.
 *
 * @since 1.0.0
 */
@property (copy, readonly) NSArray<NSURL *>			*watchedURLs;


/** @name Configuring the Event watcher */
/**
 * The URLs that we should ignore events from. 
 *
 * @return An array of <code>NSURL</code> object for the URLs which we want to ignore.
 * @discussion Events from concerning these URLs and there sub-directories will not be delivered to the delegate.
 *
 * @since 1.0.0
 */
@property (nullable, copy) NSArray<NSURL *>			*excludedURLs;

/**
 * Wheter events from sub-directories of the watched URLs should be ignored or not.
 *
 * @param flag Wheter events from sub-directories of the watched URLs shouled be ignored or not.
 * @return <code>YES</code> if events from sub-directories should be ignored, otherwise <code>NO</code>.
 *
 * @since 1.0.0
 */
@property (assign) BOOL								ignoreEventsFromSubDirectories;


#pragma mark Event identifier class methods
/** @name Current Event Identifier */
/**
 * The current event identifier.
 *
 * @return The current event identifier.
 *
 * @see FSEventsGetCurrentEventId(void)
 *
 * @since 1.0.0
 */
+ (CDEventIdentifier)currentEventIdentifier;


#pragma mark Creating CDEventsManager Objects With a Delegate
/** @name Creating CDEventsManager Objects With a Delegate */
/**
 * Returns an <code>CDEventsManager</code> object initialized with the given URLs to watch.
 *
 * @param URLs An array of URLs we want to watch.
 * @param delegate The delegate object the CDEventsManager object calls when it recieves an event.
 * @return An CDEventsManager object initialized with the given URLs to watch. 
 * @throws NSInvalidArgumentException if <em>URLs</em> is empty or points to <code>nil</code>.
 * @throws NSInvalidArgumentException if <em>delegate</em>is <code>nil</code>.
 * @throws CDEventsEventStreamCreationFailureException if we failed to create a event stream.
 *
 * @see initWithURLs:delegate:onRunLoop:
 * @see initWithURLs:delegate:onRunLoop:sinceEventIdentifier:notificationLantency:ignoreEventsFromSubDirs:excludeURLs:streamCreationFlags:
 * @see CDEventsManagerDelegate
 * @see kCDEventsDefaultEventStreamFlags
 * @see kCDEventsSinceEventNow
 *
 * @discussion Calls initWithURLs:delegate:onRunLoop:sinceEventIdentifier:notificationLantency:ignoreEventsFromSubDirs:excludeURLs:streamCreationFlags:
 * with <code>sinceEventIdentifier</code> with the event identifier for "event
 * since now", <code>notificationLatency</code> set to 3.0 seconds,
 * <code>ignoreEventsFromSubDirectories</code> set to <code>NO</code>,
 * <code>excludedURLs</code> to <code>nil</code>, the event stream creation
 * flags will be set to <code>kCDEventsDefaultEventStreamFlags</code> and
 * schedueled on the current run loop.
 *
 * @since 1.0.0
 */
- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs delegate:(id<CDEventsManagerDelegate>)delegate;

/**
 * Returns an <code>CDEventsManager</code> object initialized with the given URLs to watch and schedules the watcher on the given run loop.
 *
 * @param URLs An array of URLs we want to watch.
 * @param delegate The delegate object the CDEventsManager object calls when it recieves an event.
 * @param runLoop The run loop which the which the watcher should be schedueled on.
 * @return An CDEventsManager object initialized with the given URLs to watch.
 * @throws NSInvalidArgumentException if <em>URLs</em> is empty or points to <code>nil</code>.
 * @throws NSInvalidArgumentException if <em>delegate</em>is <code>nil</code>.
 * @throws CDEventsEventStreamCreationFailureException if we failed to create a event stream.
 *
 * @see initWithURLs:delegate:
 * @see initWithURLs:delegate:onRunLoop:sinceEventIdentifier:notificationLantency:ignoreEventsFromSubDirs:excludeURLs:streamCreationFlags:
 * @see CDEventsManagerDelegate
 * @see kCDEventsDefaultEventStreamFlags
 * @see kCDEventsSinceEventNow
 *
 * @discussion Calls initWithURLs:delegate:onRunLoop:sinceEventIdentifier:notificationLantency:ignoreEventsFromSubDirs:excludeURLs:streamCreationFlags:
 * with <code>sinceEventIdentifier</code> with the event identifier for "event
 * since now", <code>notificationLatency</code> set to 3.0 seconds,
 * <code>ignoreEventsFromSubDirectories</code> set to <code>NO</code>,
 * <code>excludedURLs</code> to <code>nil</code> and the event stream creation
 * flags will be set to <code>kCDEventsDefaultEventStreamFlags</code>.
 *
 * @since 1.0.0
 */
- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs
					delegate:(id<CDEventsManagerDelegate>)delegate
				   onRunLoop:(NSRunLoop *)runLoop;

/**
 * Returns an <code>CDEventsManager</code> object initialized with the given URLs to watch, URLs to exclude, whether events from sub-directories are ignored or not and schedules the watcher on the given run loop.
 *
 * @param URLs An array of URLs (<code>NSURL</code>) we want to watch.
 * @param delegate The delegate object the CDEventsManager object calls when it recieves an event.
 * @param runLoop The run loop which the which the watcher should be schedueled on.
 * @param sinceEventIdentifier Events that have happened after the given event identifier will be supplied.
 * @param notificationLatency The (approximate) time intervall between notifications sent to the delegate.
 * @param ignoreEventsFromSubDirs Wheter events from sub-directories of the watched URLs should be ignored or not.
 * @param exludeURLs An array of URLs that we should ignore events from. Pass <code>nil</code> if none should be excluded.
 * @param streamCreationFlags The event stream creation flags.
 * @return An CDEventsManager object initialized with the given URLs to watch, URLs to exclude, whether events from sub-directories are ignored or not and run on the given run loop.
 * @throws NSInvalidArgumentException if the parameter URLs is empty or points to <code>nil</code>.
 * @throws NSInvalidArgumentException if <em>delegate</em>is <code>nil</code>.
 * @throws CDEventsEventStreamCreationFailureException if we failed to create a event stream.
 *
 * @see initWithURLs:delegate:
 * @see initWithURLs:delegate:onRunLoop:
 * @see ignoreEventsFromSubDirectories
 * @see excludedURLs
 * @see CDEventsManagerDelegate
 * @see FSEventStreamCreateFlags
 *
 * @discussion To ask for events "since now" pass the return value of
 * currentEventIdentifier as the parameter <code>sinceEventIdentifier</code>.
 * CDEventStreamCreationFailureException should be extremely rare.
 *
 * @since 1.0.0
 */
- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs
					delegate:(id<CDEventsManagerDelegate>)delegate
				   onRunLoop:(NSRunLoop *)runLoop
		sinceEventIdentifier:(CDEventIdentifier)sinceEventIdentifier
		notificationLantency:(CFTimeInterval)notificationLatency
	 ignoreEventsFromSubDirs:(BOOL)ignoreEventsFromSubDirs
				 excludeURLs:(nullable NSArray<NSURL *> *)exludeURLs
		 streamCreationFlags:(CDEventsEventStreamCreationFlags)streamCreationFlags;

#pragma mark Creating CDEventsManager Objects With a Block
/** @name Creating CDEventsManager Objects With a Block */
/**
 * Returns an <code>CDEventsManager</code> object initialized with the given URLs to watch.
 *
 * @param URLs An array of URLs we want to watch.
 * @param block The block which the CDEventsManager object executes when it recieves an event.
 * @return An CDEventsManager object initialized with the given URLs to watch. 
 * @throws NSInvalidArgumentException if <em>URLs</em> is empty or points to <code>nil</code>.
 * @throws NSInvalidArgumentException if <em>delegate</em>is <code>nil</code>.
 * @throws CDEventsEventStreamCreationFailureException if we failed to create a event stream.
 *
 * @see initWithURLs:delegate:onRunLoop:
 * @see initWithURLs:delegate:onRunLoop:sinceEventIdentifier:notificationLantency:ignoreEventsFromSubDirs:excludeURLs:streamCreationFlags:
 * @see CDEventsEventBlock
 * @see kCDEventsDefaultEventStreamFlags
 * @see kCDEventsSinceEventNow
 *
 * @discussion Calls initWithURLs:block:onRunLoop:sinceEventIdentifier:notificationLantency:ignoreEventsFromSubDirs:excludeURLs:streamCreationFlags:
 * with <code>sinceEventIdentifier</code> with the event identifier for "event
 * since now", <code>notificationLatency</code> set to 3.0 seconds,
 * <code>ignoreEventsFromSubDirectories</code> set to <code>NO</code>,
 * <code>excludedURLs</code> to <code>nil</code>, the event stream creation
 * flags will be set to <code>kCDEventsDefaultEventStreamFlags</code> and
 * schedueled on the current run loop.
 *
 * @since head
 */
- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs block:(CDEventsEventBlock)block;

/**
 * Returns an <code>CDEventsManager</code> object initialized with the given URLs to watch and schedules the watcher on the given run loop.
 *
 * @param URLs An array of URLs we want to watch.
 * @param block The block which the CDEventsManager object executes when it recieves an event.
 * @param runLoop The run loop which the which the watcher should be schedueled on.
 * @return An CDEventsManager object initialized with the given URLs to watch.
 * @throws NSInvalidArgumentException if <em>URLs</em> is empty or points to <code>nil</code>.
 * @throws NSInvalidArgumentException if <em>delegate</em>is <code>nil</code>.
 * @throws CDEventsEventStreamCreationFailureException if we failed to create a event stream.
 *
 * @see initWithURLs:delegate:
 * @see initWithURLs:delegate:onRunLoop:sinceEventIdentifier:notificationLantency:ignoreEventsFromSubDirs:excludeURLs:streamCreationFlags:
 * @see CDEventsEventBlock
 * @see kCDEventsDefaultEventStreamFlags
 * @see kCDEventsSinceEventNow
 *
 * @discussion Calls initWithURLs:delegate:onRunLoop:sinceEventIdentifier:notificationLantency:ignoreEventsFromSubDirs:excludeURLs:streamCreationFlags:
 * with <code>sinceEventIdentifier</code> with the event identifier for "event
 * since now", <code>notificationLatency</code> set to 3.0 seconds,
 * <code>ignoreEventsFromSubDirectories</code> set to <code>NO</code>,
 * <code>excludedURLs</code> to <code>nil</code> and the event stream creation
 * flags will be set to <code>kCDEventsDefaultEventStreamFlags</code>.
 *
 * @since head
 */
- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs
					   block:(CDEventsEventBlock)block
				   onRunLoop:(NSRunLoop *)runLoop;

/**
 * Returns an <code>CDEventsManager</code> object initialized with the given URLs to watch, URLs to exclude, whether events from sub-directories are ignored or not and schedules the watcher on the given run loop.
 *
 * @param URLs An array of URLs (<code>NSURL</code>) we want to watch.
 * @param block The block which the CDEventsManager object executes when it recieves an event.
 * @param runLoop The run loop which the which the watcher should be schedueled on.
 * @param sinceEventIdentifier Events that have happened after the given event identifier will be supplied.
 * @param notificationLatency The (approximate) time intervall between notifications sent to the delegate.
 * @param ignoreEventsFromSubDirs Wheter events from sub-directories of the watched URLs should be ignored or not.
 * @param exludeURLs An array of URLs that we should ignore events from. Pass <code>nil</code> if none should be excluded.
 * @param streamCreationFlags The event stream creation flags.
 * @return An CDEventsManager object initialized with the given URLs to watch, URLs to exclude, whether events from sub-directories are ignored or not and run on the given run loop.
 * @throws NSInvalidArgumentException if the parameter URLs is empty or points to <code>nil</code>.
 * @throws NSInvalidArgumentException if <em>delegate</em>is <code>nil</code>.
 * @throws CDEventsEventStreamCreationFailureException if we failed to create a event stream.
 *
 * @see initWithURLs:delegate:
 * @see initWithURLs:delegate:onRunLoop:
 * @see ignoreEventsFromSubDirectories
 * @see excludedURLs
 * @see CDEventsEventBlock
 * @see FSEventStreamCreateFlags
 *
 * @discussion To ask for events "since now" pass the return value of
 * currentEventIdentifier as the parameter <code>sinceEventIdentifier</code>.
 * CDEventStreamCreationFailureException should be extremely rare.
 *
 * @since head
 */
- (instancetype)initWithURLs:(NSArray<NSURL *> *)URLs
					   block:(CDEventsEventBlock)block
				   onRunLoop:(NSRunLoop *)runLoop
		sinceEventIdentifier:(CDEventIdentifier)sinceEventIdentifier
		notificationLantency:(CFTimeInterval)notificationLatency
	 ignoreEventsFromSubDirs:(BOOL)ignoreEventsFromSubDirs
				 excludeURLs:(nullable NSArray<NSURL *> *)exludeURLs
		 streamCreationFlags:(CDEventsEventStreamCreationFlags)streamCreationFlags NS_DESIGNATED_INITIALIZER;

#pragma mark Flush methods
/** @name Flushing Events */
/**
 * Flushes the event stream synchronously.
 *
 * Flushes the event stream synchronously by sending events that have already occurred but not yet delivered.
 *
 * @see flushAsynchronously
 *
 * @since 1.0.0
 */
- (void)flushSynchronously;

/**
 * Flushes the event stream asynchronously.
 *
 * Flushes the event stream asynchronously by sending events that have already occurred but not yet delivered.
 *
 * @see flushSynchronously
 *
 * @since 1.0.0
 */
- (void)flushAsynchronously;

#pragma mark Misc methods
/** @name Events Description */
/**
 * Returns a NSString containing the description of the current event stream.
 *
 * @return A NSString containing the description of the current event stream.
 *
 * @see FSEventStreamCopyDescription
 *
 * @discussion For debugging only.
 *
 * @since 1.0.0
 */
- (NSString *)streamDescription;

@end

NS_ASSUME_NONNULL_END

