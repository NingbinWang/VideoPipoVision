#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include <vector>

class ResourceManager
{

public:
    ResourceManager();
    ~ResourceManager();

    bool AddResource(const char* name, void* ptr);
    bool RemoveResource(const char* name);
    void* GetResource(const char* name);
    void SetDefault(void* ptr);

private:
    typedef struct ResourceNode
    {
        const char* name;
        void* ptr;

        bool operator==(const struct ResourceNode n) const
        {
            return (this->name == n.name && this->ptr == n.ptr);
        }
    } ResourceNode_t;

private:
    std::vector<ResourceNode_t> NodePool;
    void* DefaultPtr;
    bool SearchNode(const char* name, ResourceNode_t* node);
};

#endif

