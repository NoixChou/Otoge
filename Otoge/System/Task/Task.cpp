#include "Task.h"

Task::Task(const std::string& taskName)
{
    this->TaskName_ = taskName;
    Logger_ = std::make_shared<Logger>(this->TaskName_);
}

Task::~Task()
{
    isLiving = false;
    isRunning = false;
    isInitialized = false;

    Logger_->Info("タスク開放");
}

bool Task::Initialize(int taskID)
{
    //ID = taskID;
    isInitialized = true;
    isLiving = true;
    isRunning = true;

    return true;
}

bool Task::Terminate()
{
    isTerminated = true;

    isRunning = false;

    return true;
}

void Task::Pause()
{
    isRunning = false;
}

void Task::Resume()
{
    isRunning = true;
}

std::string Task::GetName() const
{
    return TaskName_;
}


/*int Task::GetTaskID() const
{
    if (!CanRunning()) return -1;

    return ID;
}*/


bool Task::CanRunning() const
{
    return isInitialized && isLiving;
}

bool Task::IsLiving() const
{
    return isLiving;
}

bool Task::IsTerminated() const
{
    return isTerminated;
}

bool Task::IsInit() const
{
    return isInitialized;
}

bool Task::IsRunning() const
{
    return isRunning;
}


float Task::GetTickSpeed() const
{
    if (!CanRunning()) return -1;
    return tickSpeed;
}

void Task::SetTickSpeed(float tickSpeed)
{
    this->tickSpeed = tickSpeed;
}


bool Task::HasLifespan() const
{
    return hasLifespan;
}

float Task::GetLifespan() const
{
    if (!CanRunning()) return -1;

    return lifespan;
}


void Task::SetLifespan(float lifespan)
{
    this->lifespan = lifespan;
}

void Task::ChildUpdate(float deltaTime)
{
    
}

bool Task::AddChildTask(const TaskPointer& task)
{
    childrenQueues.push_back(task);
    task->parentTask = weak_from_this();
    //Logger_->Debug(TaskName_ + "タスクキュー追加 キュー数:" + std::to_string(childrenQueues.size()));
    return task->Initialize(static_cast<int>(childrenQueues.size()) - 1);
}

std::vector<Task::TaskPointer>& Task::GetChildren()
{
	return children;
}

std::vector<Task::TaskPointer>& Task::GetChildrenQueues()
{
	return childrenQueues;
}
