#include "TaskManager.hpp"
#include "../../Util/Window/DxSettings.hpp"
using namespace std;
using namespace std::chrono;

std::shared_ptr<TaskManager> TaskManager::Instance_ = nullptr;
std::vector< Task::TaskPointer >::iterator TaskManager::ProcessingTask_;

TaskManager::TaskManager()
{
    Logger_ = make_shared<Logger>("TaskManager");
    Logger_->Info("タスク機能 初期化完了");

    ProcessingTask_ = std::vector< std::shared_ptr<Task> >::iterator();
}

TaskManager::~TaskManager()
{
    Tasks_.clear();
    Logger_->Info("タスク機能 終了");
}


std::shared_ptr<TaskManager> TaskManager::GetInstance()
{
    return Instance_;
}

void TaskManager::CreateInstance()
{
    if(!Instance_)
    {
        Instance_.reset(new TaskManager);
    }
}

void TaskManager::DestroyInstance()
{
    Instance_.reset();
}


void TaskManager::GameExit()
{
    IsGameExit_ = true;
    Logger_->Warn("ゲームが終了します...");
}

Task::WeakTaskPointer TaskManager::GetModalTask() const
{
    return ModalTask_;
}

void TaskManager::SetModalTask()
{
    SetModalTask(*ProcessingTask_);
}

void TaskManager::SetModalTask(Task::WeakTaskPointer task)
{
    task.lock()->SetEnable(true);
    ModalTask_ = task;
}

void TaskManager::UnsetModalTask()
{
    if (ModalTask_.expired()) return;

    auto l_lockedModalTask = ModalTask_.lock();
    l_lockedModalTask->SetEnable(l_lockedModalTask->GetOldEnables());
    ModalTask_.reset();
}

std::vector<Task::TaskPointer>::iterator TaskManager::GetCurrentProcessTask()
{
    return ProcessingTask_;
}

int TaskManager::GetTaskCount()
{
    return static_cast<int>(Tasks_.size());
}

float TaskManager::GetFrameRate()
{
    return Fps_;
}

bool TaskManager::IsGameExit()
{
    return IsGameExit_;
}

bool TaskManager::AddTask(const shared_ptr<Task>& task)
{
    bool result = false;

    TaskQueues_.push_back(task);
    task->Initialize(static_cast<int>(TaskQueues_.size()) - 1);

    return result;
}

void TaskManager::Tick(float tickSpeed = 1.0f)
{   
    PrevClockCount_ = ClockCount_;
    ClockCount_ = high_resolution_clock::now();

    auto l_ProcessLoad = duration_cast<microseconds>(ClockCount_ - PrevClockCount_).count();

    const auto m_DeltaTime = l_ProcessLoad / 1000000.0f;

    static float fps_interval_count = 0.0f;
    fps_interval_count += m_DeltaTime;

    if (fps_interval_count > 1.0f)
    {
        Fps_ = 1000000.0f / l_ProcessLoad;
        fps_interval_count = 0.0f;
    }

    static float deltCount = 0.f;
    deltCount += m_DeltaTime;

    if (!DxSettings::doVSync || deltCount > (1.f / 60.f))
    {
        ClearDrawScreen();
        clsDx();
    }
        UpdateTasks(Tasks_, TaskQueues_, tickSpeed, m_DeltaTime);

    if (!DxSettings::doVSync || deltCount > (1.f / 60.f))
    {
        ScreenFlip();

        deltCount = 0.f;
    }
}

void TaskManager::UpdateTasks(std::vector<Task::TaskPointer>& tasks, std::vector<Task::TaskPointer>& queues, float tickSpeed, float deltaTime)
{
    if(PushQueues(tasks, queues))
    {
        UpdatePriority(tasks);
    }
	
	auto l_Task = tasks.begin();

	for (l_Task; l_Task != tasks.end(); ++l_Task)
	{
        ProcessingTask_ = l_Task;
        const auto l_BeginTime = high_resolution_clock::now();

		// タイマー更新
		float fixedDeltaTime = deltaTime * tickSpeed * (*l_Task)->GetTickSpeed();
		(*l_Task)->timerCount += fixedDeltaTime;

		// タスク処理
		if ((*l_Task)->CanRunning() && (*l_Task)->IsRunning())
		{
			(*l_Task)->Update(fixedDeltaTime);

            for (auto m_Child : (*l_Task)->GetChildren())
            {
                m_Child->parentTask = (*l_Task);
            }

            if ((*l_Task)->isAutoUpdateChildren)
            {
                UpdateTasks((*l_Task)->GetChildren(), (*l_Task)->GetChildrenQueues(), (*l_Task)->GetTickSpeed(), fixedDeltaTime);
            }
		}

		// 寿命の処理
		if ((*l_Task)->HasLifespan())
		{
			(*l_Task)->SetLifespan((*l_Task)->GetLifespan() - fixedDeltaTime);
			if ((*l_Task)->GetLifespan() < 0.0f) (*l_Task)->Terminate();
		}
        
        if (duration_cast<milliseconds>(high_resolution_clock::now() - l_BeginTime).count() > 3.f)
            Logger::LowLevelLog("Task [" + (*l_Task)->GetName() + "] is too late. 3ms<", "WARN");
	}

	l_Task = tasks.begin();
	for (l_Task; l_Task != tasks.end(); ++l_Task)
	{
		if ((*l_Task)->IsTerminated())
		{
			l_Task = tasks.erase(l_Task);
			--l_Task;
		}
	}
}

bool TaskManager::PushQueues(std::vector<Task::TaskPointer>& tasks, std::vector<Task::TaskPointer>& queues)
{
    bool l_IsAdded = false;
    for (auto task : queues)
    {
        tasks.push_back(task);
        l_IsAdded = true;
    }
    queues.clear();

    return l_IsAdded;
}

void TaskManager::UpdatePriority(std::vector<Task::TaskPointer>& tasks)
{
    std::sort(tasks.begin(), tasks.end(), [](const Task::TaskPointer & a, const Task::TaskPointer & b)
        {
            return a->GetPriority() < b->GetPriority();
        });
}
