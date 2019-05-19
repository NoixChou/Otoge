#include "CursorDrawer.hpp"

CursorDrawer::CursorDrawer() : Scene("CursorDrawer")
{
    SetPriority(100.f);
    Logger_->Debug("カーソル描画 初期化完了");
}

CursorDrawer::~CursorDrawer()
{
}

void CursorDrawer::SceneUpdate(float deltaTime)
{
}

void CursorDrawer::Draw()
{
}
