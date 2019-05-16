#include "KeyboardManager.hpp"
std::shared_ptr<KeyboardManager> KeyboardManager::Instance_ = nullptr;

KeyboardManager::KeyboardManager() : Task("KeyboardManager")
{
    KeyInputs_.reset(new char[256], std::default_delete<char[]>());
    Logger_->Info("キーボード入力管理 初期化完了");
}

bool KeyboardManager::ValidateKeyRange(int key)
{
    return true;
}

KeyboardManager::~KeyboardManager()
{
    //delete[] KeyInputs_;
    Logger_->Info("キーボード入力管理 終了");
}

std::shared_ptr<KeyboardManager> KeyboardManager::GetInstance()
{
    return Instance_;
}

void KeyboardManager::CreateInstance()
{
    if(!Instance_)
    {
        Instance_.reset(new KeyboardManager);
    }
}

void KeyboardManager::DestroyInstance()
{
    Instance_.reset();
}

void KeyboardManager::Update(float deltaTime)
{
    char buf[256];
    GetHitKeyStateAll(buf);
    for(int i = 0; i < 256; i++)
    {
        if(buf[i])
        { // 現在押されている
            if(KeyInputs_.get()[i] == 0) KeyInputs_.get()[i] = 1; // 押し始め
            else if(KeyInputs_.get()[i] == 1) KeyInputs_.get()[i] = 2; // 押している
        }
            // 現在押されていない
        else if(KeyInputs_.get()[i] == 1 || KeyInputs_.get()[i] == 2) KeyInputs_.get()[i] = 3; // 離した
        else KeyInputs_.get()[i] = 0; // 離している
    }
}

char KeyboardManager::GetKeyStatus(int key)
{
    if(!ValidateKeyRange(key)) return false;
    return KeyInputs_.get()[key];
}

bool KeyboardManager::IsDownKey(int key)
{
    if(!ValidateKeyRange(key)) return false;
    return KeyInputs_.get()[key] == 1;
}

bool KeyboardManager::IsHoldKey(int key)
{
    if(!ValidateKeyRange(key)) return false;
    return KeyInputs_.get()[key] == 2;
}

bool KeyboardManager::IsReleaseKey(int key)
{
    if(!ValidateKeyRange(key)) return false;
    return KeyInputs_.get()[key] == 3;
}
