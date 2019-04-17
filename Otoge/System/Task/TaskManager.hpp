#pragma once

#include "Task.h"
#include "../../Util/Debug/Logger.h"

class TaskManager
{
private:
    std::shared_ptr<Logger> Logger_;

    std::vector< std::shared_ptr<Task> > Tasks_;
    std::vector< std::shared_ptr<Task> > TaskQueues_;
    std::vector< std::shared_ptr<Task> >::iterator ProcessingTask_;

    std::chrono::high_resolution_clock::time_point ClockCount_ = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point PrevClockCount_ = std::chrono::high_resolution_clock::now();

    bool IsGameExit_ = false;
	bool IsGameRestart_ = false;

    int FrameCount = 0;
    int StartCount = 0;
    float FPS = 0.f;

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
    bool AddTask(const std::shared_ptr<Task> &task);
    template<class T> bool AddTaskByTypename()
    {
        return AddTask(std::make_shared<T>());
        /*Logger_->Debug("タスク追加 ID:" + std::to_string(Tasks_.size()));
        T t = new T;
        Tasks_.push_back(t);
        return t->Initialize(Tasks_.size() - 1);*/
    }

    void Tick(float tickSpeed);
	static void UpdateTasks(std::vector<std::shared_ptr<Task>>& tasks, std::vector<std::shared_ptr<Task>>& queues, float tickSpeed, float deltaTime);
};
