#include "ResourceManager.h"
#include "Logger.h"
#include <algorithm>
#include <string.h>

ResourceManager::ResourceManager()
{
    DefaultPtr = nullptr;
}

ResourceManager::~ResourceManager()
{
}

/**
  * @brief  Search resource node based on name
  * @param  name: Resource Name
  * @param  node: Pointer to the resource node
  * @retval Return true if the search is successful
  */
bool ResourceManager::SearchNode(const char* name, ResourceNode_t* node)
{
    for (auto iter : NodePool)
    {
        if (strcmp(name, iter.name) == 0)
        {
            *node = iter;
            return true;
        }
    }
    return false;
}

/**
  * @brief  Add resources to the resource pool
  * @param  name: Resource Name
  * @param  ptr: Pointer to the resource
  * @retval Return true if the addition is successful
  */
bool ResourceManager::AddResource(const char* name, void* ptr)
{
    ResourceNode_t node;
    if (SearchNode(name, &node))
    {
        LOG_WARNING("Resource: %s was register\n", name);
        return false;
    }

    node.name = name;
    node.ptr = ptr;
    NodePool.push_back(node);

    LOG_INFO("Resource: %s[0x%p] add success\n", node.name, node.ptr);

    return true;
}

/**
  * @brief  Remove resources from the resource pool
  * @param  name: Resource Name
  * @retval Return true if the removal is successful
  */
bool ResourceManager::RemoveResource(const char* name)
{
    ResourceNode_t node;
    if (!SearchNode(name, &node))
    {
        LOG_ERROR("Resource: %s was not found\n", name);
        return false;
    }

    auto iter = std::find(NodePool.begin(), NodePool.end(), node);

    if (iter == NodePool.end())
    {
        LOG_ERROR("Resource: %s was not found\n", name);
        return false;
    }

    NodePool.erase(iter);

    LOG_INFO("Resource: %s remove success\n", name);

    return true;
}

/**
  * @brief  Get resource address
  * @param  name: Resource Name
  * @retval If the acquisition is successful, return the address of the resource, otherwise return the default resource
  */
void* ResourceManager::GetResource(const char* name)
{
    ResourceNode_t node;

    if (!SearchNode(name, &node))
    {
        LOG_WARNING("Resource: %s was not found, return default[0x%p]\n", name, DefaultPtr);
        return DefaultPtr;
    }

    LOG_INFO("Resource: %s[0x%p] was found\n", name, node.ptr);

    return node.ptr;
}

/**
  * @brief  Set default resources
  * @param  ptr: Pointer to the default resource
  * @retval None
  */
void ResourceManager::SetDefault(void* ptr)
{
    DefaultPtr = ptr;
    LOG_INFO("Resource: set [0x%p] to default\n", DefaultPtr);
}
