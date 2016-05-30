#pragma once

namespace TinyStarCraft
{
 
class Mesh
{
public:
    /** Constructor */
    explicit Mesh(IDirect3DDevice9* D3DDevice);

    /** Destructor */
    ~Mesh();

    /** Create mesh */
    bool create(DWORD numFaces, DWORD numVertices, DWORD options, const D3DVERTEXELEMENT9* declaration);
    
    /** Retrieve the mesh pointer. */
    ID3DXMesh* getPointer() const { return mMesh; }

private:
    IDirect3DDevice9* mD3DDevice;
    ID3DXMesh* mMesh;
};

}

