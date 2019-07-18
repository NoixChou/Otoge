#include "Task.h"
#include "TaskManager.hpp"

Task::Task(const std::string& taskName)
{
    this->TaskName_ = taskName;
    Logger_ = std::make_shared<Logger>(this->TaskName_);
}

Task::~Task()
{
    IsLiving_ = false;
    IsRunning_ = false;
    IsInitialized_ = false;
    Logger_->Info("タスク開放");
}

bool Task::Initialize(int taskId)
{
    //ID = taskID;
    IsInitialized_ = true;
    IsLiving_ = true;
    IsRunning_ = true;
    OnInitialize();
    return true;
}

bool Task::Terminate()
{
    OnTerminate();
    IsTerminated_ = true;
    IsRunning_ = false;
    return true;
}

void Task::Pause()
{
    IsRunning_ = false;
}

void Task::Resume()
{
    IsRunning_ = true;
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
    return IsInitialized_ && IsLiving_;
}

void Task::SetEnable(bool enable)
{
    OldIsEnable_ = IsEnable_;
    IsEnable_ = enable;
}

bool Task::GetOldEnables() const
{
    return OldIsEnable_;
}

bool Task::IsEnable() const
{
    auto l_ModalTask = TaskManager::GetInstance()->GetModalTask();
    if(!l_ModalTask.expired())
    {
        return IsOnModal() && IsRawEnable();
    }
    return IsRawEnable();
}

bool Task::IsRawEnable() const
{
    if(!parentTask.expired())
    {
        return parentTask.lock()->IsEnable() && IsEnable_;
    }
    return IsEnable_;
}

bool Task::IsOnModal() const
{
    if(TaskManager::GetInstance()->GetModalTask().lock() == shared_from_this())
    {
        return true;
    }
    if(!parentTask.expired()) return parentTask.lock()->IsOnModal();
    return false;
}

bool Task::IsLiving() const
{
    return IsLiving_;
}

bool Task::IsTerminated() const
{
    return IsTerminated_;
}

bool Task::IsInit() const
{
    return IsInitialized_;
}

bool Task::IsRunning() const
{
    return IsRunning_;
}

float Task::GetTickSpeed() const
{
    if(!CanRunning()) return -1;
    return TickSpeed_;
}

void Task::SetTickSpeed(float tickSpeed)
{
    this->TickSpeed_ = tickSpeed;
}

float Task::GetPriority() const
{
    return RunningPriority_ - 100.f;
}

void Task::SetPriority(float priority)
{
    RunningPriority_ = priority + 100.f;
}

bool Task::HasLifespan() const
{
    return HasLifespan_;
}

float Task::GetLifespan() const
{
    if(!CanRunning()) return -1;
    return Lifespan_;
}

void Task::SetLifespan(float lifespan)
{
    this->Lifespan_ = lifespan;
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
