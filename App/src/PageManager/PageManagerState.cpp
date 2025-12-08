#include "PageManager.h"
#include "Logger.h"

 /**
   * @brief  Update page state machine
   * @param  base: Pointer to the updated page
   * @retval None
   */
void PageManager::StateUpdate(PageBase* base)
{
    if (base == nullptr)
        return;

    switch (base->priv.State)
    {
    case PageBase::PAGE_STATE_IDLE:
        LOG_INFO("Page(%s) state idle", base->Name);
        break;

    case PageBase::PAGE_STATE_LOAD:
        base->priv.State = StateLoadExecute(base);
        StateUpdate(base);
        break;

    case PageBase::PAGE_STATE_WILL_APPEAR:
        base->priv.State = StateWillAppearExecute(base);
        break;

    case PageBase::PAGE_STATE_DID_APPEAR:
        base->priv.State = StateDidAppearExecute(base);
        LOG_INFO("Page(%s) state active", base->Name);
        break;

    case PageBase::PAGE_STATE_ACTIVITY:
        LOG_INFO("Page(%s) state active break", base->Name);
        base->priv.State = PageBase::PAGE_STATE_WILL_DISAPPEAR;
        StateUpdate(base);
        break;

    case PageBase::PAGE_STATE_WILL_DISAPPEAR:
        base->priv.State = StateWillDisappearExecute(base);
        break;

    case PageBase::PAGE_STATE_DID_DISAPPEAR:
        base->priv.State = StateDidDisappearExecute(base);
        if (base->priv.State == PageBase::PAGE_STATE_UNLOAD)
        {
            StateUpdate(base);
        }
        break;

    case PageBase::PAGE_STATE_UNLOAD:
        base->priv.State = StateUnloadExecute(base);
        break;

    default:
        LOG_ERROR("Page(%s) state[%d] was NOT FOUND!", base->Name, base->priv.State);
        break;
    }
}

/**
  * @brief  Page loading status
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateLoadExecute(PageBase* base)
{
    LOG_INFO("Page(%s) state load", base->Name);

    if (base->root != nullptr)
    {
        LOG_ERROR("Page(%s) root must be nullptr", base->Name);
    }

    lv_obj_t* root_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(root_obj, LV_HOR_RES, LV_VER_RES);
    lv_obj_clear_flag(root_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_user_data(root_obj, base);

    base->root = root_obj;
    base->onViewLoad();

    if (GetIsOverAnim(GetCurrentLoadAnimType()))
    {
        PageBase* bottomPage = GetStackTopAfter();

        if (bottomPage != nullptr && bottomPage->priv.IsCached)
        {
            LoadAnimAttr_t animAttr;
            if (GetCurrentLoadAnimAttr(&animAttr))
            {
                if (animAttr.dragDir != ROOT_DRAG_DIR_NONE)
                {
                    RootEnableDrag(base->root);
                }
            }
        }
    }

    base->onViewDidLoad();

    if (base->priv.IsDisableAutoCache)
    {
        LOG_INFO("Page(%s) disable auto cache, ReqEnableCache = %d", base->Name, base->priv.ReqEnableCache);
        base->priv.IsCached = base->priv.ReqEnableCache;
    }
    else
    {
        LOG_INFO("Page(%s) AUTO cached", base->Name);
        base->priv.IsCached = true;
    }

    return PageBase::PAGE_STATE_WILL_APPEAR;
}

/**
  * @brief  The page is about to show the status
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateWillAppearExecute(PageBase* base)
{
    LOG_INFO("Page(%s) state will appear", base->Name);
    base->onViewWillAppear();
    lv_obj_clear_flag(base->root, LV_OBJ_FLAG_HIDDEN);
    SwitchAnimCreate(base);
    return PageBase::PAGE_STATE_DID_APPEAR;
}

/**
  * @brief  The status of the page display
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateDidAppearExecute(PageBase* base)
{
    LOG_INFO("Page(%s) state did appear", base->Name);
    base->onViewDidAppear();
    return PageBase::PAGE_STATE_ACTIVITY;
}

/**
  * @brief  The page is about to disappear
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateWillDisappearExecute(PageBase* base)
{
    LOG_INFO("Page(%s) state will disappear", base->Name);
    base->onViewWillDisappear();
    SwitchAnimCreate(base);
    return PageBase::PAGE_STATE_DID_DISAPPEAR;
}

/**
  * @brief  Page disappeared end state
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateDidDisappearExecute(PageBase* base)
{
    LOG_INFO("Page(%s) state did disappear", base->Name);
    if (GetCurrentLoadAnimType() == LOAD_ANIM_FADE_ON)
    {
        LOG_INFO("AnimState.TypeCurrent == LOAD_ANIM_FADE_ON, Page(%s) hidden", base->Name);
        lv_obj_add_flag(base->root, LV_OBJ_FLAG_HIDDEN);
    }
    base->onViewDidDisappear();
    if (base->priv.IsCached)
    {
        LOG_INFO("Page(%s) has cached", base->Name);
        return PageBase::PAGE_STATE_WILL_APPEAR;
    }
    else
    {
        return PageBase::PAGE_STATE_UNLOAD;
    }
}

/**
  * @brief  Page unload complete
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateUnloadExecute(PageBase* base)
{
    LOG_INFO("Page(%s) state unload", base->Name);
    if (base->root == nullptr)
    {
        LOG_WARNING("Page is loaded!");
        goto Exit;
    }

    if (base->priv.Stash.ptr != nullptr && base->priv.Stash.size != 0)
    {
        LOG_INFO("Page(%s) free stash(0x%p)[%d]", base->Name, base->priv.Stash.ptr, base->priv.Stash.size);
        //lv_mem_free(base->priv.Stash.ptr);
        base->priv.Stash.ptr = nullptr;
        base->priv.Stash.size = 0;
    }
    lv_obj_del_async(base->root);
    base->root = nullptr;
    base->priv.IsCached = false;
    base->onViewDidUnload();

Exit:
    return PageBase::PAGE_STATE_IDLE;
}
