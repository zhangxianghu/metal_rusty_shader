#include <WinApp.h>
#include <AppUtil.h>
#include <GeometryGens.h>
#include <RenderStates.h>
#include <Lights.h>
#include <Camera.h>
#include "Effects.h"
#include "Inputs.h"

//Vertex format used in sky box rendering
struct PosVertex
{
	XMFLOAT3	pos;
};

//Sky box with reflection
class SkyBoxDemo: public WinApp
{
public:
	SkyBoxDemo(HINSTANCE hInst, std::wstring title = L"D3D11 Cube Mapping", int width = 800, int height = 600);
	~SkyBoxDemo();

	bool	Init();							//Initialization
	bool	OnResize();						//Window size changed
	bool	Update(float timeDelt);			//Update for each frame
	bool	Render();						//Scene rendering for each frame

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	bool BuildSRVs();
	bool BuildBuffers();

private:
	ID3D11Buffer	*m_VBSky;
	ID3D11Buffer	*m_IBSky;

	ID3D11Buffer	*m_VBSphere;
	ID3D11Buffer	*m_IBSphere;

	ID3D11ShaderResourceView	*m_texCubeSRV;		//Cube map used as sky box and reflection

	GeoGen::MeshData	m_skySphere;
	GeoGen::MeshData	m_sphere;

	XMFLOAT4X4		m_worldSphere;
	XMFLOAT4X4		m_invWorldTranspose;

	Lights::Material	m_material;

	Lights::DirLight	m_dirLights[3];

	Camera		m_camera;
	
	POINT		m_lastPos;
};

SkyBoxDemo::SkyBoxDemo(HINSTANCE hInst, std::wstring title, int width, int height):WinApp(hInst,title,width,height),
	m_VBSky(NULL),
	m_IBSky(NULL),
	m_VBSphere(NULL),
	m_IBSphere(NULL),
	m_texCubeSRV(NULL)
{
	m_camera.SetPosition(0.f,0.f,-3.f);

	//3 directional lights
	//Main Light
	m_dirLights[0].ambient  =	XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_dirLights[0].diffuse  =	XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_dirLights[0].specular =	XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_dirLights[0].dir		 =	XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
	//light2
	m_dirLights[1].ambient  =	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_dirLights[1].diffuse  =	XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	m_dirLights[1].specular =	XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_dirLights[1].dir		 =	XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);
	//light3
	m_dirLights[2].ambient  =	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_dirLights[2].diffuse  =	XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_dirLights[2].specular =	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_dirLights[2].dir	     =	XMFLOAT3(0.0f, -0.707f, -0.707f);

	m_material.ambient = XMFLOAT4(0.5f,0.5f,0.5f,1.f);
	m_material.diffuse = XMFLOAT4(1.f,1.f,1.f,1.f);
	m_material.specular = XMFLOAT4(0.3f,0.3f,0.3f,30.f);
	m_material.reflection = XMFLOAT4(0.8f,0.8f,0.8f,0.8f);

	XMStoreFloat4x4(&m_worldSphere,XMMatrixIdentity());
	XMStoreFloat4x4(&m_invWorldTranspose,XMMatrixIdentity());
}

SkyBoxDemo::~SkyBoxDemo()
{
	SafeRelease(m_VBSky);
	SafeRelease(m_IBSky);
	SafeRelease(m_VBSphere);
	SafeRelease(m_IBSphere);
	SafeRelease(m_texCubeSRV);

	Effects::ReleaseAll();
	InputLayouts::ReleaseAll();
	RenderStates::ReleaseAll();
}

bool SkyBoxDemo::Init()
{
	if(!WinApp::Init())
		return false;

	if(!Effects::InitAll(m_d3dDevice))
		return false;
	if(!InputLayouts::InitAll(m_d3dDevice))
		return false;
	if(!RenderStates::InitAll(m_d3dDevice))
		return false;
	if(!BuildBuffers())
		return false;
	if(!BuildSRVs())
		return false;

	return true;
}

bool SkyBoxDemo::BuildBuffers()
{
	//For sky
	GeoGen::CreateSphere(100.f,30,30,m_skySphere);

	D3D11_BUFFER_DESC descSky = {0};
	descSky.ByteWidth = sizeof(PosVertex) * m_skySphere.vertices.size();
	descSky.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	descSky.Usage = D3D11_USAGE_DEFAULT;

	std::vector<PosVertex> verticesSky(m_skySphere.vertices.size());
	for(UINT i=0; i<m_skySphere.vertices.size(); ++i)
	{
		verticesSky[i].pos = m_skySphere.vertices[i].pos;
	}

	D3D11_SUBRESOURCE_DATA vDataSky;
	vDataSky.pSysMem = &verticesSky[0];
	vDataSky.SysMemPitch = 0;
	vDataSky.SysMemSlicePitch = 0;
	if(FAILED(m_d3dDevice->CreateBuffer(&descSky,&vDataSky,&m_VBSky)))
	{
		MessageBox(NULL,L"Create Vertex Buffer failed!",L"Error",MB_OK);
		return false;
	}

	D3D11_BUFFER_DESC iDescSky = {0};
	iDescSky.ByteWidth = sizeof(UINT) * m_skySphere.indices.size();
	iDescSky.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iDescSky.Usage = D3D11_USAGE_DEFAULT;
	std::vector<UINT> indicesSky(m_skySphere.indices.size());
	for(UINT i=0; i<m_skySphere.indices.size(); ++i)
	{
		indicesSky[i] = m_skySphere.indices[i];
	}
	D3D11_SUBRESOURCE_DATA iDataSky;
	iDataSky.pSysMem = &indicesSky[0];
	iDataSky.SysMemPitch = 0;
	iDataSky.SysMemSlicePitch = 0;
	if(FAILED(m_d3dDevice->CreateBuffer(&iDescSky,&iDataSky,&m_IBSky)))
	{
		MessageBox(NULL,L"Create Index Buffer failed!",L"Error",MB_OK);
		return false;
	}

	//For sphere
	GeoGen::CreateSphere(0.8f,30,30,m_sphere);

	D3D11_BUFFER_DESC descSphere = {0};
	descSphere.ByteWidth = sizeof(Vertex::Basic32) * m_sphere.vertices.size();
	descSphere.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	descSphere.Usage = D3D11_USAGE_DEFAULT;

	std::vector<Vertex::Basic32> verticesSphere(m_sphere.vertices.size());
	for(UINT i=0; i<m_sphere.vertices.size(); ++i)
	{
		verticesSphere[i].pos = m_sphere.vertices[i].pos;
		verticesSphere[i].normal = m_sphere.vertices[i].normal;
		verticesSphere[i].tex = m_sphere.vertices[i].tex;
	}

	D3D11_SUBRESOURCE_DATA vDataSphere;
	vDataSphere.pSysMem = &verticesSphere[0];
	vDataSphere.SysMemPitch = 0;
	vDataSphere.SysMemSlicePitch = 0;
	if(FAILED(m_d3dDevice->CreateBuffer(&descSphere,&vDataSphere,&m_VBSphere)))
	{
		MessageBox(NULL,L"Create Vertex Buffer failed!",L"Error",MB_OK);
		return false;
	}

	D3D11_BUFFER_DESC iDescSphere = {0};
	iDescSphere.ByteWidth = sizeof(UINT) * m_sphere.indices.size();
	iDescSphere.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iDescSphere.Usage = D3D11_USAGE_DEFAULT;
	std::vector<UINT> indicesSphere(m_sphere.indices.size());
	for(UINT i=0; i<m_sphere.indices.size(); ++i)
	{
		indicesSphere[i] = m_sphere.indices[i];
	}
	D3D11_SUBRESOURCE_DATA iDataSphere;
	iDataSphere.pSysMem = &indicesSphere[0];
	iDataSphere.SysMemPitch = 0;
	iDataSphere.SysMemSlicePitch = 0;
	if(FAILED(m_d3dDevice->CreateBuffer(&iDescSphere,&iDataSphere,&m_IBSphere)))
	{
		MessageBox(NULL,L"Create Index Buffer failed!",L"Error",MB_OK);
		return false;
	}
	return true;
}

bool SkyBoxDemo::BuildSRVs()
{
	if(FAILED(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,L"textures/snowcube1024.dds",0,0,&m_texCubeSRV,0)))
	{
		MessageBox(NULL,L"Create SRV failed!",L"Error",MB_OK);
		return false;
	}

	return true;
}

bool SkyBoxDemo::OnResize()
{
	if(!WinApp::OnResize())
		return false;

	m_camera.SetLens(XM_PI*0.25f,1.f*m_clientWidth/m_clientHeight,1.f,1000.f);

	return true;
}

bool SkyBoxDemo::Update(float delta)
{
	if(KeyDown('A'))
	{
		m_camera.Strafe(-6.f*delta);
	}
	else if(KeyDown('D'))
	{
		m_camera.Strafe(6.f*delta);
	}
	if(KeyDown('W'))
	{
		m_camera.Walk(6.f*delta);
	}
	else if(KeyDown('S'))
	{
		m_camera.Walk(-6.f*delta);
	}

	//Update per frame shader variables
	Effects::fxBasic->SetLights(m_dirLights);
	Effects::fxBasic->SetEyePos(m_camera.GetPosition());

	m_camera.UpdateView();

	return true;
}

bool SkyBoxDemo::Render()
{
	m_deviceContext->ClearRenderTargetView(m_renderTargetView,reinterpret_cast<const float*>(&Colors::Silver));
	m_deviceContext->ClearDepthStencilView(m_depthStencilView,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.f,0);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//Two techniques: one for sphere, the other for sky box
	ID3DX11EffectTechnique *mainTech = Effects::fxBasic->fxLight3ReflectionTech;
	ID3DX11EffectTechnique *tech = Effects::fxSkyBox->fxSkyBoxTech;
	D3DX11_TECHNIQUE_DESC mainTechDesc,techDesc;
	mainTech->GetDesc(&mainTechDesc);
	tech->GetDesc(&techDesc);
	
	//Begin rendering sphere
	m_deviceContext->IASetInputLayout(InputLayouts::basic32);
	UINT stride1 = sizeof(Vertex::Basic32);
	UINT offset1 = 0;
	m_deviceContext->IASetVertexBuffers(0,1,&m_VBSphere,&stride1,&offset1);
	m_deviceContext->IASetIndexBuffer(m_IBSphere,DXGI_FORMAT_R32_UINT,0);
	for(UINT i=0; i<mainTechDesc.Passes; ++i)
	{
		//Update per object shader variables
		XMMATRIX world = XMLoadFloat4x4(&m_worldSphere);
		XMMATRIX wvp = world * m_camera.ViewProjection();
		XMMATRIX invWorldTrans = XMLoadFloat4x4(&m_invWorldTranspose);
		Effects::fxBasic->SetWorldMatrix(world);
		Effects::fxBasic->SetWorldViewProjMatrix(wvp);
		Effects::fxBasic->SetWorldInvTransposeMatrix(invWorldTrans);
		Effects::fxBasic->SetCubeMap(m_texCubeSRV);
		Effects::fxBasic->SetMaterial(m_material);

		mainTech->GetPassByIndex(i)->Apply(0,m_deviceContext);
		m_deviceContext->DrawIndexed(m_sphere.indices.size(),0,0);
		//Restore render states for other renderings
		m_deviceContext->RSSetState(0);
	}
	
	//Begin rendering sky box
	m_deviceContext->IASetInputLayout(InputLayouts::pos);
	UINT stride2 = sizeof(PosVertex);
	UINT offset2 = 0;
	m_deviceContext->IASetVertexBuffers(0,1,&m_VBSky,&stride2,&offset2);
	m_deviceContext->IASetIndexBuffer(m_IBSky,DXGI_FORMAT_R32_UINT,0);
	for(UINT i=0; i<techDesc.Passes; ++i)
	{
		//Update per obejct shader variables
		XMFLOAT3 eyePos = m_camera.GetPosition();
		XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
		XMMATRIX WVP = XMMatrixMultiply(T, m_camera.ViewProjection());
		Effects::fxSkyBox->SetWorldViewProjMatrix(WVP);
		Effects::fxSkyBox->SetCubeMap(m_texCubeSRV);

		tech->GetPassByIndex(i)->Apply(0,m_deviceContext);
		m_deviceContext->DrawIndexed(m_skySphere.indices.size(),0,0);
		//Restore render states for other renderings
		m_deviceContext->RSSetState(0);
	}
	
	m_swapChain->Present(0,0);

	return true;
}

void SkyBoxDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_lastPos.x = x;
	m_lastPos.y = y;
	SetCapture(m_hWnd);
}

void SkyBoxDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void SkyBoxDemo::OnMouseMove(WPARAM btnState, int x, int y)
{
	if((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f * (x - m_lastPos.x));
		float dy = XMConvertToRadians(0.25f * (y - m_lastPos.y));

		m_camera.Pitch(dy);
		m_camera.RotateY(dx);
	}

	m_lastPos.x = x;
	m_lastPos.y = y;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	SkyBoxDemo sky(hInstance);
	if(!sky.Init())
		return -1;

	return sky.Run(); 
}