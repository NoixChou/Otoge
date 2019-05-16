#pragma once
#include "../Task/Task.h"

class KeyboardManager : public Task
{
private:
    std::shared_ptr<char> KeyInputs_; // 0:入力されていない 1:入力された瞬間 2:入力されている 3:離された瞬間
    static std::shared_ptr<KeyboardManager> Instance_;
    KeyboardManager();
    bool ValidateKeyRange(int key);
public:
    ~KeyboardManager();
    static std::shared_ptr<KeyboardManager> GetInstance();
    static void CreateInstance();
    static void DestroyInstance();
    void Update(float deltaTime) override;
    char GetKeyStatus(int key);
    bool IsDownKey(int key);
    bool IsHoldKey(int key);
    bool IsReleaseKey(int key);
};
