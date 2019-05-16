//#include "Config.h"
/*
// 設定ファイル
namespace settings
{
    template<typename T> struct config
    {
        string key;
        T default_value;
    };

    namespace system
    {
        config<boost::none_t> c_section = { "system.", boost::none };

        namespace graphic
        {
            config<boost::none_t> c_section = { system::c_section.key + "graphic.", boost::none };


            config<bool> fullscreen = { c_section.key + "fullscreen", false };
            config<bool> vsync = { c_section.key + "vsync", false };

            namespace resolution
            {
                config<boost::none_t> c_section = { graphic::c_section.key + "resolution.", boost::none };


                config<int> width = { c_section.key + "width", GAME_SCREEN_WIDTH };
                config<int> height = { c_section.key + "height", GAME_SCREEN_HEIGHT };
            }

            namespace antialiasing
            {
                config<boost::none_t> c_section = { graphic::c_section.key + "antialiasing.", boost::none };


                config<int> sample = { c_section.key + "sample", 2 };
                config<int> quality = { c_section.key + "quality", 2 };
            }

        }

    }
}
*/
