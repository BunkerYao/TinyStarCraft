#pragma once

namespace TinyStarCraft
{

class ResourceManager;

class Resource
{
public:
    /** Constructor */
    Resource(ResourceManager* owner, const std::string& name)
        : mOwner(owner), mName(name)
    {}

    /**
    	Destructor
    @remarks
        All subclasses should properly destroy the underlying resource in destructor.
     */
    virtual ~Resource() {}

    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;

    /** Get the owner resource manager of this resource. */
    ResourceManager* getOwner() const { return mOwner; }

    /** Get the name of this resource. */
    const std::string& getName() const { return mName; }

protected:
    // Pointer to the resource manager who created this resource
    ResourceManager* mOwner;
    // Unique name of this resource.
    std::string mName;
};

};
