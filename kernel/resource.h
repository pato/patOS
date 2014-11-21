#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "atomic.h"
#include "debug.h"

enum ResourceType {
    PROCESS,
    SEMAPHORE,
    FILE,
    TABLE,
    OTHER
};

class Resource {
public:
    static Resource* ref(Resource* p) {
        if (p) {
            p->count.getThenAdd(1);
        }
        return p;
    }
    static Resource* unref(Resource* p) {
        if (p) {
            uint32_t old = p->count.getThenAdd(-1);
            if (old == 1) {
                delete p;
                return nullptr;
            }
        }
        return p;
    }

    const ResourceType type;
    Atomic32 count;

    Resource(ResourceType type) : type(type) {}
    virtual ~Resource() {}

    /* What happens when this resource is forked.
       Default behavior is to increment the count and
       return a pointer to the same resource.

       Some resources need to change this behavior
     */
    virtual Resource* forkMe() {
        return ref(this);
    }
};

typedef Resource *ResourcePtr;

class Reference {
    Resource *res;
public:
    // Reference r(&resource);
    Reference(Resource *res) : res(Resource::ref(res)) { }

    virtual ~Reference() {
        Resource::unref(res);
        res = nullptr;
    }

    // Reference r(r2);
    // Reference r = r2;
    Reference(const Reference& other) : res(Resource::ref(other.res)) { }

    // extern Reference func();
    // Reference r = func();
    Reference(Reference&& other) : res(other.res) {
        other.res = nullptr;
    }

    // r = r2;
    Reference& operator = (const Reference& other) {
        if (this->res != other.res) {
            Resource::unref(this->res);
            this->res = Resource::ref(other.res);
        }
        return *this;
    }

    Resource* operator ->() { return res; }
};

#endif
