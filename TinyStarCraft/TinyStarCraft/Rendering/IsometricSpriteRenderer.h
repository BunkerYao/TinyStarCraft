#pragma once

namespace TinyStarCraft
{

class Mesh;
class IsometricSprite;

class IsometricSpriteRenderer
{
public:
    static const int BATCH_SIZE = 32;

public:
    /** Constructor */
    IsometricSpriteRenderer();

    /** Destructor */
    ~IsometricSpriteRenderer();

    /** Initialize the renderer */
    bool initialize(IDirect3DDevice9* D3DDevice);

    /** Draw isometric sprite instances */
    void draw(const std::vector<IsometricSprite*>& sprites, IDirect3DDevice9* D3DDevice, ID3DXEffect* effect);

private:
    Mesh* mInstancesMesh;

    std::vector<D3DXMATRIX> mInstancesWorldMatrices;
    std::vector<D3DXVECTOR4> mInstancesTextureRects;
    std::vector<D3DXVECTOR4> mInstancesHeight;
};

};
