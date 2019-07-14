#pragma once
#include "../Debug/Logger.h"

class SettingManager : public std::enable_shared_from_this<SettingManager>
{
private:
    std::shared_ptr<Logger> Logger_;
    std::string FileName_;
    boost::property_tree::ptree SettingsTree_;
    boost::property_tree::ptree DefaultTree_;
    static std::shared_ptr<SettingManager> GlobalSettings_;
    static int GlobalUser_;
    bool CanProcess();
public:
    SettingManager(const std::string& fileName);
    ~SettingManager();
    bool Load(const std::string& fileName, bool autoCreate = false);
    bool Load(bool autoCreate = false);
    bool Reload();
    bool Save();
    bool Save(const std::string& fileName);
    void Close();
    void SetGlobal();
    static std::shared_ptr<SettingManager> GetGlobal();

    template< typename T >
    void SetDefault(const std::string& dataPath, T value)
    {
        Logger_->Debug("初期値セット: " + dataPath + " = " + std::to_string(value));
        if(SettingsTree_.get_optional<T>(dataPath) == boost::none)
        {
            Logger_->Info("初期化: " + dataPath);
            Set(dataPath, value);
        }
        
        //DefaultTree_.put(dataPath, value);
    }

    template<>
    void SetDefault<std::string>(const std::string& dataPath, std::string value)
    {
        Logger_->Debug("初期値セット: " + dataPath + " = " + value);
        if(SettingsTree_.get_optional<std::string>(dataPath) == boost::none)
        {
            Logger_->Info("初期化: " + dataPath);
            Set(dataPath, value);
        }
    }

    template< typename T >
    boost::optional<T> Get(const std::string& dataPath)
    {
        if(boost::optional<T> l_Value = SettingsTree_.get_optional<T>(dataPath))
        {
            Logger_->Debug("取得成功: " + dataPath + " = " + std::to_string(l_Value.get()));
            return l_Value;
        }
        Logger_->Error("取得失敗: " + dataPath);
        return boost::none;
    }

    template<>
    boost::optional<std::string> Get(const std::string& dataPath)
    {
        if(boost::optional<std::string> l_Value = SettingsTree_.get_optional<std::string>(dataPath))
        {
            Logger_->Debug("取得成功: " + dataPath + " = " + l_Value.get());
            return l_Value;
        }
        Logger_->Error("取得失敗: " + dataPath);
        return boost::none;
    }

    template< typename T >
    void Set(const std::string& dataPath, T value)
    {
        SettingsTree_.put(dataPath, value);
    }
};
