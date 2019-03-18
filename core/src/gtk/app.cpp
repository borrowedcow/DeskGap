#include <gtk/gtk.h>
#include <functional>
#include <memory>
#include <cstdlib>
#include <unordered_map>

#include "../app/app.h"
#include "util/xdg-user-dir-lookup.h"

using std::shared_ptr;
using std::function;
using std::make_shared;

namespace DeskGap {
    struct App::Impl {
    	App::EventCallbacks callbacks;
    };

    App::App(EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
    	impl_->callbacks = std::move(callbacks);
    }
    
    void App::Run() {
        impl_->callbacks.onWillFinishLaunching();
    	impl_->callbacks.onReady();
    }
    
    void App::Exit(int exitCode) {
        std::exit(exitCode);
    }

    std::string App::GetPath(PathName name) {
        static std::unordered_map<PathName, const char*> xdgDirTypeByPathName {
            { PathName::DESKTOP, "DESKTOP" },
            { PathName::DOCUMENTS, "DOCUMENTS" },
            { PathName::DOWNLOADS, "DOWNLOAD" },
            { PathName::MUSIC, "MUSIC" },
            { PathName::PICTURES, "PICTURES" },
            { PathName::VIDEOS, "VIDEOS" },
        };

        if (name == PathName::APP_DATA) {
            if (const char* xdgConfigHome = getenv("XDG_CONFIG_HOME"); xdgConfigHome != nullptr) {
                return xdgConfigHome;
            }
            else {
                return std::string(g_get_home_dir()) + "/.config";
            }
            return g_get_home_dir();
        }
        else if (name == PathName::HOME) {
            return g_get_home_dir();
        }
        else if (name == PathName::TEMP) {
            return g_get_tmp_dir();
        }
        else {
            char* cresult = xdg_user_dir_lookup(xdgDirTypeByPathName[name]);
            std::string result(cresult);
            free(cresult);
            return std::move(result);
        }
    }
    
    App::~App() = default;
}

