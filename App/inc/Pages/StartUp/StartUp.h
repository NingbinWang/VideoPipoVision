#ifndef _STARTUP_H_
#define _STARTUP_H_

#include "StartUpView.h"
#include "StartUpModel.h"

namespace Page
{

    class Startup : public PageBase
    {
    public:

    public:
        Startup();
        virtual ~Startup();

        virtual void onCustomAttrConfig();
        virtual void onViewLoad();//页面开始加载
        virtual void onViewDidLoad();
        virtual void onViewWillAppear();
        virtual void onViewDidAppear();//页面即将显示
        virtual void onViewWillDisappear();//页面即将消失
        virtual void onViewDidDisappear();//页面消失完成
        virtual void onViewDidUnload();//页面卸载完成

    private:
        static void onTimer(lv_timer_t* timer);

        StartupView View;
        StartupModel Model;
    };

}

#endif
