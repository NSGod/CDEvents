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
 * @headerfile CDEventsManagerDelegate.h CDEvents/CDEventsManagerDelegate.h

 * 
 * A protocol that that delegates of CDEventsManager conform to. Inspired and based
 * upon the open source project SCEvents created by Stuart Connolly
 * http://stuconnolly.com/projects/code/
 */

@class CDEventsManager;
@class CDEvent;

NS_ASSUME_NONNULL_BEGIN

/**
 * The CDEventsManagerDelegate protocol defines the required methods implemented by delegates of CDEvents objects.
 *
 * @see CDEventsManager
 * @see CDEvent
 *
 * @since 1.0.0
 */
@protocol CDEventsManagerDelegate

@required
/**
 * The method called by the <code>CDEventsManager</code> object on its delegate object.
 *
 * @param URLWatcher The <code>CDEventsManager</code> object which the event was recieved thru.
 * @param event The event data.
 *
 * @see CDEventsManager
 * @see CDEvent
 *
 * @discussion Conforming objects' implementation of this method will be called
 * whenever an event occurs. The instance of CDEventsManager which received the event
 * and the event itself are passed as parameters.
 *
 * @since 1.0.0
 */
- (void)eventsManager:(CDEventsManager *)aManager eventOccurred:(CDEvent *)event;

@end

NS_ASSUME_NONNULL_END
