#include "TaskManager.hpp"

using namespace std;
using namespace std::chrono;

TaskManager *TaskManager::Instance_ = nullptr;

TaskManager::TaskManager()
{
    Logger_ = make_shared<Logger>("TaskManager");
    Logger_->Info("タスク機能 初期化完了");

    ProcessingTask_ = std::vector< std::shared_ptr<Task> >::iterator();
}

TaskManager::~TaskManager()
{
    Logger_->Info("タスク機能 終了");
}


TaskManager *TaskManager::GetInstance()
{
    return Instance_;
}

void TaskManager::CreateInstance()
{
    if(!Instance_)
    {
        Instance_ = new TaskManager;
    }
}

void TaskManager::DestroyInstance()
{
    delete Instance_;
}


void TaskManager::GameExit()
{
    IsGameExit_ = true;
    Logger_->Warn("ゲームが終了します...");
}

int TaskManager::GetTaskCount()
{
    return Tasks_.size();
}

float TaskManager::GetFrameRate()
{
    return FPS;
}

bool TaskManager::IsGameExit()
{
    return IsGameExit_;
}

bool TaskManager::AddTask(const shared_ptr<Task>& task)
{
    bool result = false;

    /*if (ProcessingTask_ != std::vector< std::shared_ptr<Task> >::iterator())
    {
        Logger_->Info("プロセス内からのAddTask呼び出し");
        result = (*ProcessingTask_)->AddChildTask(task);
    }
    else*/ {
        TaskQueues_.push_back(task);
        Logger_->Debug("タスクキュー追加 キュー数:" + to_string(TaskQueues_.size()));

        task->Initialize(TaskQueues_.size() - 1);
    }

    return result;
}

void TaskManager::Tick(float tickSpeed = 1.0f)
{   
    PrevClockCount_ = ClockCount_;
    ClockCount_ = high_resolution_clock::now();

    const auto m_DeltaTime = duration_cast<microseconds>(ClockCount_ - PrevClockCount_).count() / 1000000.0f;

    static float fps_interval_count = 0.0f;
    fps_interval_count += m_DeltaTime;

    if (fps_interval_count > 1.0f)
    {
        FPS = 1000000.0f / duration_cast<microseconds>(ClockCount_ - PrevClockCount_).count();
        fps_interval_count = 0.0f;
    }

    for (auto& task : TaskQueues_)
    {
        Tasks_.push_back(task);
        Logger_->Debug("タスク追加 タスク数:" + std::to_string(Tasks_.size()));
    }
    TaskQueues_.clear();

    auto m_Task = Tasks_.begin();

    ClearDrawScreen();
    clsDx();
    
    for(m_Task; m_Task != Tasks_.end(); ++m_Task)
    {
        // タイマー更新
        float fixedDeltaTime = m_DeltaTime * tickSpeed * (*m_Task)->GetTickSpeed();
        (*m_Task)->timerCount += fixedDeltaTime;

        // タスク処理
        if ((*m_Task)->CanRunning() && (*m_Task)->IsRunning())
        {
            (*m_Task)->Update(fixedDeltaTime);
            if((*m_Task)->isAutoUpdateChildren)
                (*m_Task)->ChildUpdate(fixedDeltaTime);
        }

        // 寿命の処理
        if((*m_Task)->HasLifespan())
        {
            (*m_Task)->SetLifespan((*m_Task)->GetLifespan() - fixedDeltaTime);
            if ((*m_Task)->GetLifespan() < 0.0f) (*m_Task)->Terminate();
        }

        if ((*m_Task)->IsTerminated())
        {
            m_Task = Tasks_.erase(m_Task);
            --m_Task;
        }
    }

    //m_Task = std::vector< std::shared_ptr<Task> >::iterator();
    ScreenFlip();
}
