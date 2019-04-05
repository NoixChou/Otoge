#pragma once
#include "../../Util/Debug/Logger.h"

class Task
{
private:
    //int ID = -1; // タスクID

protected:
    std::string TaskName_ = ""; // 名前

    std::shared_ptr<Logger> Logger_;

    bool isLiving = false; // 生きていて、実行可能か(未使用)
    bool isTerminated = false; // 終了中か
    bool isInitialized = false; // 初期化されているか
    bool isRunning = false; // Updateを実行するか

    float tickSpeed = 1.0f; // 処理スピード (deltaTimeが補正される)

    bool hasLifespan = false; // 寿命があるか
    float lifespan = 0.f; // 寿命

public:
    bool isAutoUpdateChildren = true; // 子タスクのUpdateを自動で実行するか

    std::vector<std::shared_ptr<Task>> children; // 子タスク
    std::vector<std::shared_ptr<Task>> childrenQueues;

    float timerCount = 0.f; // deltaTime蓄積

    Task(const std::string &taskName);
    virtual ~Task();

    bool Initialize(int taskID);
    virtual void Update(float deltaTime) = 0;
    
    bool Terminate();
    void Pause();
    void Resume();


    //int GetTaskID() const;

    std::string GetName() const;

    bool CanRunning() const;
    
    bool IsLiving() const;
    bool IsTerminated() const;
    bool IsInit() const;
    bool IsRunning() const;
    
    float GetTickSpeed() const;
    void SetTickSpeed(float tickSpeed);

    bool HasLifespan() const;
    float GetLifespan() const;
    void SetLifespan(float lifespan);

    // 子タスク
    void ChildUpdate(float deltaTime);
    bool AddChildTask(const std::shared_ptr<Task> &task);
	std::vector<std::shared_ptr<Task>>& GetChildren();
	std::vector<std::shared_ptr<Task>>& GetChildrenQueues();
    
};
