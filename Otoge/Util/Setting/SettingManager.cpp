#include "SettingManager.h"

using namespace std;

SettingManager *SettingManager::GlobalSettings_ = nullptr;

bool SettingManager::CanProcess()
{
    return true;
}

SettingManager::SettingManager(const std::string &fileName)
{
    Logger_ = make_shared<Logger>("SettingManager");
    FileName_ = fileName;
}

SettingManager::~SettingManager()
{
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
            std::ofstream f(fileName);
            f.write("{}", 2);
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
        GlobalSettings_ = this;
        Logger_->Warn("グローバル設定になりました。");
    }else
    {
        Logger_->Error("既にグローバル設定が存在するのにSetGlobal()が呼ばれました");
    }
}

SettingManager* SettingManager::GetGlobal()
{
    return GlobalSettings_;
}

/*template <class T> void SettingManager::SetDefault(const std::string &dataPath, T value)
{
    DefaultTree_.put(dataPath, value);
}

template<class T> T SettingManager::Get(const std::string &dataPath)
{
    if (boost::optional<T> value = SettingsTree_.get_optional<T>(dataPath))
    {
        return value.get();
    }
    return nullptr;
}

template <class T> void SettingManager::Set(const std::string &dataPath, T value)
{
    SettingsTree_.put(dataPath, value);
}
*/
