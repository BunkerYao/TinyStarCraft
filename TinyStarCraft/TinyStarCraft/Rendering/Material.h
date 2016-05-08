#pragma once

namespace TinyStarCraft
{

class Material
{
public:
    /**
     *	Constructor
     */
    Material(ID3DXEffect* effect, int Id)
        : mEffect(effect), mId(Id), mParamBlockHandle(NULL)
    {}

    virtual ~Material()
    {
        if (mParamBlockHandle)
            mEffect->DeleteParameterBlock(mParamBlockHandle);
    }

    /**
     *	Check if two material are the same by checking the Id of them.
     */
    bool operator==(const Material& other) const
    {
        return mId == other.mId;
    }

    /**
     *	Override operator< for material sort.
     */
    bool operator<(const Material& other) const
    {
        return mId < other.mId;
    }

    /**
     *	Get the effect interface.
     */
    ID3DXEffect* getEffect() const
    {
        return mEffect;
    }

    /**
     *	Get the Id of this material.
     */
    int getId() const
    {
        return mId;
    }

    /**
     *	Apply this material's parameter to the effect before rendering.
     */
    bool apply()
    {
        if (mParamBlockHandle) {
            return SUCCEEDED(mEffect->ApplyParameterBlock(mParamBlockHandle));
        }
        return false;
    }

    /**
     *	Begin to change parameters of this material.
     */
    bool beginParameterChange()
    {
        if (mParamBlockHandle) {
            // Delete the previous parameter block
            mEffect->DeleteParameterBlock(mParamBlockHandle);
        }

        return SUCCEEDED(mEffect->BeginParameterBlock());
    }

    /**
     *	Commit parameter changes between beginParameterChange() and endParameterChange().
     */
    void endParameterChange()
    {
        mParamBlockHandle = mEffect->EndParameterBlock();
    }

private:
    ID3DXEffect* mEffect;
    D3DXHANDLE mParamBlockHandle;
    int mId;
};

}
