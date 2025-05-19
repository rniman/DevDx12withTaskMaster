# DirectX12 Triangle Example with Taskmaster&cursor

DX12로 삼각형을 그리는 최소 예제 프로젝트입니다.

## 빌드 방법
- Visual Studio 2022 이상
- Windows 10 SDK(DirectX 12 포함) 필요

## 실행 방법
- 솔루션 빌드 후 실행

## Taskmaster 사용 .env 관련

- 다음처럼 루트 폴더에 .env파일에 api키 작성
ANTHROPIC_API_KEY=your_anthropic_api_key_here       # Required: Format: sk-ant-api03-...
PERPLEXITY_API_KEY=your_perplexity_api_key_here     # Optional: Format: pplx-...
OPENAI_API_KEY=your_openai_api_key_here             # Optional, for OpenAI/OpenRouter models. Format: sk-proj-...
GOOGLE_API_KEY=your_google_api_key_here             # Optional, for Google Gemini models.
MISTRAL_API_KEY=your_mistral_key_here               # Optional, for Mistral AI models.
XAI_API_KEY=YOUR_XAI_KEY_HERE                       # Optional, for xAI AI models.
AZURE_OPENAI_API_KEY=your_azure_key_here            # Optional, for Azure OpenAI models (requires endpoint in .taskmasterconfig).