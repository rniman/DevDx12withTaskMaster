#include "pch.h"
#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "d3dx12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// WinMain: 프로그램 진입점
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // 1. 윈도우 클래스 등록 및 생성
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0, 0, hInstance, nullptr, nullptr, nullptr, nullptr, L"DX12WindowClass", nullptr };
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindow(wc.lpszClassName, L"DirectX 12 Triangle", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720, nullptr, nullptr, wc.hInstance, nullptr);
    ShowWindow(hWnd, nCmdShow);

    // 2. DXGI 팩토리 및 디바이스 생성
    ComPtr<IDXGIFactory6> dxgiFactory;
    CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    ComPtr<ID3D12Device> device;
    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

    // 3. 삼각형 정점 데이터 및 Vertex Buffer 생성
    struct Vertex {
        float position[3];
        float color[4];
    };
    Vertex triangleVertices[] = {
        { { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };
    const UINT vertexBufferSize = sizeof(triangleVertices);
    ComPtr<ID3D12Resource> vertexBuffer;
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));
    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, triangleVertices, vertexBufferSize);
    vertexBuffer->Unmap(0, nullptr);
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex);
    vertexBufferView.SizeInBytes = vertexBufferSize;

    // 4. 셰이더 컴파일 및 PSO 생성
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;
    ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3DCompileFromFile(L"Shaders/BasicVS.hlsl", nullptr, nullptr, "main", "vs_5_1", 0, 0, &vertexShader, &errorBlob);
    if (FAILED(hr)) { if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer()); }
    hr = D3DCompileFromFile(L"Shaders/BasicPS.hlsl", nullptr, nullptr, "main", "ps_5_1", 0, 0, &pixelShader, &errorBlob);
    if (FAILED(hr)) { if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer()); }
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    ComPtr<ID3D12RootSignature> rootSignature;
    device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    ComPtr<ID3D12PipelineState> pipelineState;
    device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

    // 5. 커맨드 큐, 스왑체인, RTV, 커맨드 리스트, 펜스 등 초기화
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ComPtr<ID3D12CommandQueue> commandQueue;
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Width = 1280;
    swapChainDesc.Height = 720;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    ComPtr<IDXGISwapChain1> swapChain1;
    dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
    ComPtr<IDXGISwapChain3> swapChain;
    swapChain1.As(&swapChain);
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ComPtr<ID3D12DescriptorHeap> rtvHeap;
    device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
    UINT rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    ComPtr<ID3D12Resource> renderTargets[2];
    for (UINT i = 0; i < 2; ++i) {
        swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += i * rtvDescriptorSize;
        device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
    }
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    ComPtr<ID3D12GraphicsCommandList> commandList;
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList));
    ComPtr<ID3D12Fence> fence;
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    UINT64 fenceValue = 1;

    // 6. 메시지 루프 및 렌더링
    bool running = true;
    MSG msg = {};
    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) running = false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
        commandAllocator->Reset();
        commandList->Reset(commandAllocator.Get(), nullptr);
        D3D12_VIEWPORT viewport = {0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f};
        D3D12_RECT scissorRect = {0, 0, 1280, 720};
        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissorRect);
        commandList->SetPipelineState(pipelineState.Get());
        commandList->SetGraphicsRootSignature(rootSignature.Get());
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += backBufferIndex * rtvDescriptorSize;
        FLOAT clearColor[] = { 0.1f, 0.2f, 0.0f, 1.0f }; // 초록+파랑 계열
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            renderTargets[backBufferIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        commandList->DrawInstanced(3, 1, 0, 0);
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            renderTargets[backBufferIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
        commandList->Close();
        ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
        commandQueue->ExecuteCommandLists(1, ppCommandLists);
        swapChain->Present(1, 0);
        commandQueue->Signal(fence.Get(), fenceValue);
        if (fence->GetCompletedValue() < fenceValue) {
            fence->SetEventOnCompletion(fenceValue, fenceEvent);
            WaitForSingleObject(fenceEvent, INFINITE);
        }
        fenceValue++;
    }
    CloseHandle(fenceEvent);
    UnregisterClass(wc.lpszClassName, hInstance);
    return 0;
}
