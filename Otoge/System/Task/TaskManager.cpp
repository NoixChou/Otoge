#include "TaskManager.hpp"

using namespace std;
using namespace std::chrono;

std::shared_ptr<TaskManager> TaskManager::Instance_ = nullptr;

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

    const auto m_DeltaTime = duration_cast<microseconds>(ClockCount_ - PrevClockCount_).count() / 1000000.0f;

    static float fps_interval_count = 0.0f;
    fps_interval_count += m_DeltaTime;

    if (fps_interval_count > 1.0f)
    {
        Fps_ = 1000000.0f / duration_cast<microseconds>(ClockCount_ - PrevClockCount_).count();
        fps_interval_count = 0.0f;
    }

    ClearDrawScreen();
    clsDx();
    
	UpdateTasks(Tasks_, TaskQueues_, tickSpeed, m_DeltaTime);
	//Logger_->Info("Ticked");
    //m_Task = std::vector< std::shared_ptr<Task> >::iterator();
    ScreenFlip();
}

void TaskManager::UpdateTasks(std::vector<Task::TaskPointer>& tasks, std::vector<Task::TaskPointer>& queues, float tickSpeed, float deltaTime)
{
    bool l_IsAdded = false;
	for (auto task : queues)
	{
		tasks.push_back(task);
        l_IsAdded = true;
	}
	queues.clear();

    if(l_IsAdded)
    {
        UpdatePriority(tasks);
    }
	
	auto m_Task = tasks.begin();

	for (m_Task; m_Task != tasks.end(); ++m_Task)
	{
		//Logger::LowLevelLog("処理", (*m_Task)->GetName());
		// タイマー更新
		float fixedDeltaTime = deltaTime * tickSpeed * (*m_Task)->GetTickSpeed();
		(*m_Task)->timerCount += fixedDeltaTime;

		// タスク処理
		if ((*m_Task)->CanRunning() && (*m_Task)->IsRunning())
		{
			(*m_Task)->Update(fixedDeltaTime);

            for (auto m_Child : (*m_Task)->GetChildren())
            {
                m_Child->parentTask = (*m_Task);
            }

            if ((*m_Task)->isAutoUpdateChildren)
            {
                UpdateTasks((*m_Task)->GetChildren(), (*m_Task)->GetChildrenQueues(), (*m_Task)->GetTickSpeed(), fixedDeltaTime);
            }
		}

		// 寿命の処理
		if ((*m_Task)->HasLifespan())
		{
			(*m_Task)->SetLifespan((*m_Task)->GetLifespan() - fixedDeltaTime);
			if ((*m_Task)->GetLifespan() < 0.0f) (*m_Task)->Terminate();
		}
	}

	m_Task = tasks.begin();
	for (m_Task; m_Task != tasks.end(); ++m_Task)
	{
		if ((*m_Task)->IsTerminated())
		{
			m_Task = tasks.erase(m_Task);
			--m_Task;
		}
	}
}

void TaskManager::UpdatePriority(std::vector<Task::TaskPointer>& tasks)
{
    std::sort(tasks.begin(), tasks.end(), [](const Task::TaskPointer & a, const Task::TaskPointer & b)
        {
            return a->GetPriority() < b->GetPriority();
        });
}
