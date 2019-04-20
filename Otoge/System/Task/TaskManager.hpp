#pragma once

#include "Task.h"
#include "../../Util/Debug/Logger.h"

class TaskManager
{
private:
    std::shared_ptr<Logger> Logger_;

    std::vector< Task::TaskPointer > Tasks_;
    std::vector< Task::TaskPointer > TaskQueues_;
    std::vector< Task::TaskPointer >::iterator ProcessingTask_;

    std::chrono::high_resolution_clock::time_point ClockCount_ = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point PrevClockCount_ = std::chrono::high_resolution_clock::now();

    bool IsGameExit_ = false;
	bool IsGameRestart_ = false;

    int FrameCount_ = 0;
    int StartCount_ = 0;
    float Fps_ = 0.f;

protected:
	static std::shared_ptr<TaskManager> Instance_;
    TaskManager();

public:
    ~TaskManager();

    static std::shared_ptr<TaskManager> GetInstance();
    static void CreateInstance();
    static void DestroyInstance();

    float GetFrameRate();
    bool IsGameExit();
    void GameExit();

    int GetTaskCount();
    bool AddTask(const Task::TaskPointer &task);
    template<class T> bool AddTaskByTypename()
    {
        return AddTask(std::make_shared<T>());
        /*Logger_->Debug("タスク追加 ID:" + std::to_string(Tasks_.size()));
        T t = new T;
        Tasks_.push_back(t);
        return t->Initialize(Tasks_.size() - 1);*/
    }

    void Tick(float tickSpeed);
	static void UpdateTasks(std::vector<Task::TaskPointer>& tasks, std::vector<Task::TaskPointer>& queues, float tickSpeed, float deltaTime);
    static void UpdatePriority(std::vector<Task::TaskPointer>& tasks);
};
