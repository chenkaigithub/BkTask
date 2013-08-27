//
//  BkTask.h
//  BkNetwork
//
//  Created by Pierre Monod-Broca on 23/01/12.
//  Copyright (c) 2012 Backelite. All rights reserved.
//

#import <Foundation/Foundation.h>

@class BkTask;
@class BkTaskContent;
@protocol BkTaskStep;
typedef NSOperation <BkTaskStep> BkStepOperation;
typedef void (^BkTaskCompletion)(BkTask *task, id output);
typedef void (^BkTaskFailure)(BkTask *task, NSError *error);

/**
 * Asynchronous task group.
 *
 * Meant particularly to handle url request when used with [BkURLRequestOperation]
 *
 * @ingroup Task
 * @see BkTask, BkURLRequestOperation (in BkNetwork), BkTask(BkURLRequestOperation) (in BkNetwork)
 */
@interface BkTask : NSObject <NSCopying>

@property (strong, nonatomic) BkStepOperation *initialStep;
@property (strong, nonatomic, readonly) BkTaskContent *content;

/**
 *  Application defined value that can be used to identify a task.
 */
@property (strong, nonatomic) id taskId;

/**
 *  Is set to YES when the task starts, then to NO when it stops, whether it is cancelled, done, or has failed. KVO compliant. Thread safe.
 */
@property (readonly) BOOL isExecuting;

/**
 *  Is set to NO when the task stops, whether it is cancelled, done, or has failed. KVO compliant. Thread safe.
 */
@property (readonly) BOOL isFinished;

/**
 *  Is set to YES when the method -cancel is called. KVO compliant. Thread safe.
 */
@property (readonly) BOOL isCancelled;

/**
 *  Designated initializer.
 *
 *  @return An initialized task.
 */
- (id) init;

/**
 *  Adds a new step to receiver after the last one.
 *
 *  @param aStep The step to add.
 */
- (void) addStep:(BkStepOperation *)aStep;

/**
 *  Adds a new step to receiver after another one.
 *
 *  @param aStep        The step to add.
 *  @param previousStep Reference to the step after which the new step will be added.
 *  @discussion Call with previousStep set to nil to add it as the first step.
 */
- (void) insertStep:(BkStepOperation *)aStep afterStep:(BkStepOperation *)previousStep;

/**
 *  Adds a step before another one. Raises an exception if nextStep is already running.
 *
 *  @param aStep    The step to add.
 *  @param nextStep Reference to the step before which the new step will be added.
 */
- (void) insertStep:(BkStepOperation *)aStep beforeStep:(BkStepOperation *)nextStep;


/**
 *  Adds a block to be called on completion.
 *
 *  @param target An object intended to be used to remove the related completion block when necessary. It is unretained but beware that the same object could be retained by the completion block.
 *  @param completionBlock A block called when the task is finished.
 *  @see \ref removeTargetCompletions:
 */
- (void) addTarget:(id)target completion:(BkTaskCompletion)completionBlock;

/**
 *  Removes all completion blocks related to a target. If all completion blocks are removed, the task is automatically canceled.
 *
 *  @param target The object on which completion blocks have been added.
 *  @see \ref addTarget:completion:
 */
- (void) removeTargetCompletions:(id)target;

/**
 *  Adds a block to be called on failure.
 *
 *  @param target       An object intended to be used to remove the related failure block when necessary. It is unretained but beware that the same object could be retained by the failure block.
 *  @param failureBlock A block called when the task fails to complete.
 *  @see \ref removeTargetFailures:
 */
- (void) addTarget:(id)target failure:(BkTaskFailure)failureBlock;

/**
 *  Removes the failure handlers registered for target.
 *
 *  @param target The object on which failure blocks have been added.
 *  @see \ref addTarget:failure:
 */
- (void) removeTargetFailures:(id)target;

/**
 *  Begins the execution of the task.
 */
- (void) start;

/**
 * Remove all targets and tries to cancel the task. In most case you should rather remove the target that is no longer interested in the result.
 * @see \ref removeTargetCompletions:
 */
- (void) cancel;


@end

@protocol BkTaskStep <NSObject, NSCopying>

@required

/**
 *  The setter must be used exclusively by the owning task
 */
@property (unsafe_unretained, nonatomic) BkTask *task;

/**
 *  The setter must be used exclusively by the owning task
 */
@property (unsafe_unretained, nonatomic) BkStepOperation *previousStep;

/**
 *  The setter must be used exclusively by the owning task
 */
@property (unsafe_unretained, nonatomic) BkStepOperation *nextStep;

/**
 *  The content to be processed by the step
 */
@property (strong, nonatomic) BkTaskContent *content;

/**
 *  Getter for the step error object. When error is different from nil, the task will fail and call the failure block.
 *
 *  @return The step error object.
 */
- (NSError *) error;

@optional
/**
 *   Getter for the operation queue executing the step. This method does not return the main queue as steps are executed in background
 *
 *  @return The operation queue executing the step.
 */
- (NSOperationQueue *) stepOperationQueue;

@end
