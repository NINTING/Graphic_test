#include"d3dUtility.h"
#include<D3DX10Math.h>
#include<D3DX10.h>
#include<d3dx9.h>

IDirect3DDevice9 *Device = 0;
int Height = 600;
int Width = 800;
struct Vertex {
	float _x, _y, _z;
	Vertex() {};
	Vertex(float x, float y, float z) :_x(x), _y(y), _z(z) {};
	static const DWORD FVF;

};
const DWORD Vertex::FVF = D3DFVF_XYZ;

ID3DXMesh* Teapot = 0;
IDirect3DVertexBuffer9* VB = 0;
IDirect3DIndexBuffer9* IB = 0;
bool setup() { 
	
	D3DXCreateTeapot(Device, &Teapot, 0);

	//创造顶点缓存，索引缓存
	Device->CreateVertexBuffer(8 * sizeof(Vertex),D3DUSAGE_WRITEONLY , Vertex::FVF,D3DPOOL_MANAGED,&VB,0);
	Device->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &IB, 0);
	
	Vertex *vertices;
	VB->Lock(0, 0, (void**)&vertices, 0);
	
	vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
	vertices[1] = Vertex(-1.0f, 1.0f, -1.0f);
	vertices[2] = Vertex(1.0f, 1.0f, -1.0f);
	vertices[3] = Vertex(1.0f, -1.0f, -1.0f);
	vertices[4] = Vertex(-1.0f, -1.0f, 1.0f);
	vertices[5] = Vertex(-1.0f, 1.0f, 1.0f);
	vertices[6] = Vertex(1.0f, 1.0f, 1.0f);
	vertices[7] = Vertex(1.0f, -1.0f, 1.0f);
	VB->Unlock();
	
	WORD*indices = 0;
	IB->Lock(0, 0, (void**)&indices, 0);
	//front side
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	//back side
	indices[6] = 4; indices[7] = 6; indices[8] = 5;
	indices[9] = 4; indices[10] = 7; indices[11] = 6;
	
	//left side
	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;

	//right side
	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;

	//top
	indices[24] = 1; indices[25] = 5; indices[26] = 6;
	indices[27] = 1; indices[28] = 6; indices[29] = 2;


	//bottom
	indices[30] = 4; indices[31] = 0; indices[32] = 3;
	indices[33] = 4; indices[34] = 3; indices[35] = 7;

	IB->Unlock();
	
	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);//初始位置
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);	//目标位置
	D3DXVECTOR3 up(0.0f,1.0f,0.0f);			//向上
	D3DXMATRIX V;
	//获得取景变换矩阵
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	//设置投影矩阵
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI*0.5,					//垂直场弧度
		float(Width) / (float)Height,	//纵横比
		1.0f,							//摄影机距近平面的距离
		1000.0f							//摄影机距远平面的距离
	);
	Device->SetTransform(D3DTS_PROJECTION,&proj);

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);//渲染模式为线框模式
	return true;
}



void Cleanup(){
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);
	Teapot->Release();
	Teapot = 0;
}

bool Display(float timeDelta) {
	if (Device) {
		D3DXMATRIX  Ry,Rx;


		D3DXMatrixRotationX(&Rx, 0);
		static float y = 0.0f;
		
		D3DXMatrixRotationY(&Ry, y);
		y += timeDelta;
		if (y >= 6.28)y = 0.0f;
		
		D3DXMATRIX p =  Ry*Rx;

		Device->SetTransform(D3DTS_WORLD, &p);
		
		Device->Clear(0,						//pRect中的矩形数目
			0,									//所要清除的屏幕矩形数组 pRect
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,	//指定所要清除的表面	（目标表面，深度缓存）
			0xffffffff,							//指定颜色
			1.0f,								//深度缓存z
			0);									//模板缓存的值

		Device->BeginScene();
		Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		Device->SetIndices(IB);
		Device->SetFVF(Vertex::FVF);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
		Teapot->DrawSubset(0);
		Device->EndScene();

		Device->Present(0, 0, 0, 0);
	}
	return true;
}

LRESULT CALLBACK d3d::WndProc(HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam)

{

	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	if (!d3d::InitD3D(hinstance,640, 480, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}

