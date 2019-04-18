#pragma once
#include "../../Util/Debug/Logger.h"
#include <cstdarg>

class Task : public std::enable_shared_from_this<Task>
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
    using TaskPointer = std::shared_ptr<Task>;
    using WeakTaskPointer = std::weak_ptr<Task>;

    bool isAutoUpdateChildren = true; // 子タスクのUpdateを自動で実行するか

    WeakTaskPointer parentTask; // 親タスク
    std::vector<TaskPointer> children; // 子タスク
    std::vector<TaskPointer> childrenQueues;

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
    bool AddChildTask(const TaskPointer& task);
	std::vector<TaskPointer>& GetChildren();
	std::vector<TaskPointer>& GetChildrenQueues();
};
