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
    float RunningPriority_ = 0.f; // 実行優先順位
    bool OldIsEnable_ = true; // 変更前の有効性
    bool IsEnable_ = true; // 有効か(各タスクで使用)
    bool IsLiving_ = false; // 生きていて、実行可能か(未使用)
    bool IsTerminated_ = false; // 終了中か
    bool IsInitialized_ = false; // 初期化されているか
    bool IsRunning_ = false; // Updateを実行するか
    float TickSpeed_ = 1.0f; // 処理スピード (deltaTimeが補正される)
    bool HasLifespan_ = false; // 寿命があるか
    float Lifespan_ = 0.f; // 寿命
public:
    using TaskPointer = std::shared_ptr<Task>;
    using WeakTaskPointer = std::weak_ptr<Task>;
    bool isAutoUpdateChildren = true; // 子タスクのUpdateを自動で実行するか
    WeakTaskPointer parentTask; // 親タスク
    std::vector<TaskPointer> children; // 子タスク
    std::vector<TaskPointer> childrenQueues;
    float timerCount = 0.f; // deltaTime蓄積
    Task(const std::string& taskName);
    virtual ~Task();
    bool Initialize(int taskID);
    virtual void Update(float deltaTime) = 0;
    bool Terminate();
    void Pause();
    void Resume();


    //int GetTaskID() const;
    std::string GetName() const;
    bool CanRunning() const;
    void SetEnable(bool enable);
    bool GetOldEnables() const;
    bool IsEnable() const;
    bool IsRawEnable() const;
    bool IsOnModal() const;
    bool IsLiving() const;
    bool IsTerminated() const;
    bool IsInit() const;
    bool IsRunning() const;
    float GetTickSpeed() const;
    void SetTickSpeed(float tickSpeed);
    float GetPriority() const;
    void SetPriority(float priority);
    bool HasLifespan() const;
    float GetLifespan() const;
    void SetLifespan(float lifespan);

    // 子タスク
    void ChildUpdate(float deltaTime);
    bool AddChildTask(const TaskPointer& task);
    std::vector<TaskPointer>& GetChildren();
    std::vector<TaskPointer>& GetChildrenQueues();
};
