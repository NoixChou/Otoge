#include "SettingManager.h"
#include "../Encoding/EncodingConverter.h"

using namespace std;

std::shared_ptr<SettingManager> SettingManager::GlobalSettings_ = nullptr;
int SettingManager::GlobalUser_ = 0;

bool SettingManager::CanProcess()
{
    return true;
}

SettingManager::SettingManager(const std::string &fileName)
{
    Logger_ = make_shared<Logger>("SettingManager");
    FileName_ = fileName;
    GlobalUser_++;
}

SettingManager::~SettingManager()
{
    GlobalUser_--;
    if(GlobalUser_ == 0)
    {
        Logger_->Info("ユーザーがいないためグローバル設定を初期化します");
        GlobalSettings_.reset();
       // GlobalSettings_ = nullptr;
    }
}

bool SettingManager::Load(const std::string &fileName, bool autoCreate)
{
    Logger_->Info(fileName + " 読み込み中...");

    try {
        read_json(fileName, SettingsTree_);
        Logger_->Info("読み込み成功。");
        return true;
    }catch(boost::property_tree::json_parser::json_parser_error e)
    {
        Logger_->Critical(fileName + "の読み込みに失敗しました。\n詳細: " + e.what());
        if(autoCreate)
        {
            Logger_->Warn("ファイルが見つからないので作成します...");
            std::wofstream f(fileName);
            f.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
            f.write(encoding::ConvertUtf8ToUtf16("{}").c_str(), 2);
            f.close();

            if (Load(fileName, false))
            {
                //SettingsTree_.push_back(std::make_pair("", DefaultTree_));
                Save();
                Logger_->Warn("ファイルを作成しました。");
                return true;
            }
        }
    }

    return false;
}

bool SettingManager::Load(bool autoCreate)
{
    return Load(FileName_, autoCreate);
}

bool SettingManager::Reload()
{
    Logger_->Debug(FileName_ + " 再読込");
    return Load(FileName_);
}

bool SettingManager::Save()
{
    return Save(FileName_);
}

bool SettingManager::Save(const std::string &fileName)
{
    Logger_->Info(fileName + "保存中...");

    try {
        write_json(fileName, SettingsTree_);
        Logger_->Info(FileName_ + " 保存");
        return true;
    } catch(boost::property_tree::json_parser::json_parser_error e)
    {
        Logger_->Critical(fileName + "の保存に失敗しました。\n詳細: " + e.what());
    }
    return false;
}

void SettingManager::Close()
{
    FileName_ = "";
}

void SettingManager::SetGlobal()
{
    if(!GlobalSettings_)
    {
        GlobalSettings_ = shared_from_this();
        Logger_->Warn("グローバル設定になりました。");
    }else
    {
        Logger_->Error("既にグローバル設定が存在するのにSetGlobal()が呼ばれました");
    }
}

std::shared_ptr<SettingManager> SettingManager::GetGlobal()
{
    return GlobalSettings_;
}