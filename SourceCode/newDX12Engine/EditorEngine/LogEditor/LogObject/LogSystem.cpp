#include "LogSystem.h"

FEditorLogSystem* FEditorLogSystem::LogSystem = NULL;

FEditorLogSystem::FEditorLogSystem()
{
    bAutoScroll = true;
}

FEditorLogSystem* FEditorLogSystem::Get()
{
    if (!LogSystem)
    {
        LogSystem = new FEditorLogSystem();
    }

    return LogSystem;
}

void FEditorLogSystem::Destory()
{
    if (LogSystem)
    {
        delete LogSystem;
        LogSystem = NULL;
    }
}
// 初始化字体配置以提高清晰度
void FEditorLogSystem::InitializeFonts(float FontSize, float DPIScale)
{
    ImGuiIO& io = ImGui::GetIO();

    // 清除现有字体
    io.Fonts->Clear();

    // 字体配置 - 关键：启用抗锯齿和像素对齐
    ImFontConfig fontConfig;
    fontConfig.OversampleH = 3; // 水平过采样，提高清晰度
    fontConfig.OversampleV = 3; // 垂直过采样，提高清晰度
    fontConfig.PixelSnapH = true; // 像素对齐，减少模糊
    fontConfig.GlyphExtraSpacing.x = 0.0f; // 字符间距
    fontConfig.RasterizerMultiply = 1.0f; // 光栅化倍增器

    // 根据DPI缩放调整字体大小
    float scaledFontSize = FontSize * DPIScale;

    // 方案1: 使用默认字体（改进配置）
    io.Fonts->AddFontDefault(&fontConfig);

    // 方案2: 如果你有自定义字体文件，取消下面的注释
    // 推荐使用高质量等宽字体，如 Consolas, Courier New, Source Code Pro
    /*
    const char* fontPath = "C:/Windows/Fonts/consola.ttf"; // Windows Consolas
    // const char* fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"; // Linux
    ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath, scaledFontSize, &fontConfig);
    if (font == NULL)
    {
        // 如果加载失败，回退到默认字体
        io.Fonts->AddFontDefault(&fontConfig);
    }
    */

    // 构建字体纹理 - 使用更高质量的设置
    io.Fonts->Build();

    // 设置全局字体缩放（如果需要）
    // io.FontGlobalScale = DPIScale;
}


void FEditorLogSystem::Clear()
{
    TextBuff.clear();
    LineOffsets.clear();
    LineOffsets.push_back(0);
    PreLineColor.clear();
    PreLineColor.push_back(e_error::SIMPLE_C_LOG);
}

void FEditorLogSystem::HandleBackstageLog(e_error InColorID, int InOldSize)
{
    // 获取日志类型
    char error_str[64] = { 0 };
    get_error_str(InColorID, error_str);

    // 合并字符串
    TextBuff.append(error_str);

    // 重置字符串
    ResetLineOffsets(InColorID, InOldSize);
}

void FEditorLogSystem::AddLog(const char* Fmt, ...)
{
    // 获取上一次字体多少
    int TextSize = TextBuff.size();

    // 可变参数
    va_list Args;
    va_start(Args, Fmt);
    TextBuff.appendfv(Fmt, Args);
    va_end(Args);

    HandleBackstageLog(SIMPLE_C_LOG, TextSize);
}

void FEditorLogSystem::AddError(const char* Fmt, ...)
{
    int TextSize = TextBuff.size();

    va_list Args;
    va_start(Args, Fmt);
    TextBuff.appendfv(Fmt, Args);
    va_end(Args);

    HandleBackstageLog(SIMPLE_C_ERROR, TextSize);
}

void FEditorLogSystem::AddWarning(const char* Fmt, ...)
{
    int TextSize = TextBuff.size();

    va_list Args;
    va_start(Args, Fmt);
    TextBuff.appendfv(Fmt, Args);
    va_end(Args);

    HandleBackstageLog(SIMPLE_C_WARNING, TextSize);
}

void FEditorLogSystem::AddSuccess(const char* Fmt, ...)
{
    int TextSize = TextBuff.size();

    va_list Args;
    va_start(Args, Fmt);
    TextBuff.appendfv(Fmt, Args);
    va_end(Args);

    HandleBackstageLog(SIMPLE_C_SUCCESS, TextSize);
}

void FEditorLogSystem::Draw(float DeltaTime)
{
    if (PreLineColor.Size != LineOffsets.Size)
    {
        return;
    }

    if (!ImGui::Begin("Log"))
    {
        ImGui::End();
        return;
    }

    // 按钮布局
    {
        if (ImGui::BeginPopup("LogOptions"))
        {
            ImGui::Checkbox("Auto Scroll", &bAutoScroll);
            ImGui::EndPopup();
        }

        ImGui::SameLine();

        // 操作日志按钮
        if (ImGui::Button("LogOptions"))
        {
            ImGui::OpenPopup("LogOptions");
        }

        ImGui::SameLine();

        if (ImGui::Button("ClearAllLog"))
        {
            Clear();
        }

        ImGui::SameLine();

        if (ImGui::Button("CopyAllLog"))
        {
            ImGui::LogToClipboard();
        }

        ImGui::SameLine();

        TextFilter.Draw("LogFilter", -100.0f);
    }

    const char* TextBuffStart = TextBuff.begin();
    const char* TextBuffEnd = TextBuff.end();

    ImGui::Separator();

    ImGui::BeginChild("LogScrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    // 日志筛选器
    if (TextFilter.IsActive())
    {
        for (int i = 0; i < LineOffsets.Size; i++)
        {
            const char* LineStart = TextBuffStart + LineOffsets[i];
            const char* LineEnd = (i + 1 < LineOffsets.Size) ? (TextBuffStart + LineOffsets[i + 1] - 1) : TextBuffEnd;

            if (TextFilter.PassFilter(LineStart, LineEnd))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, GetColor(PreLineColor[i]));
                ImGui::TextUnformatted(LineStart, LineEnd);
                ImGui::PopStyleColor();
            }
        }
    }
    else
    {
        ImGuiListClipper Clipper;
        Clipper.Begin(LineOffsets.Size);
        while (Clipper.Step())
        {
            for (int i = Clipper.DisplayStart; i < Clipper.DisplayEnd; i++)
            {
                const char* LineStart = TextBuffStart + LineOffsets[i];
                const char* LineEnd = (i + 1 < LineOffsets.Size) ? (TextBuffStart + LineOffsets[i + 1] - 1) : TextBuffEnd;;

                ImGui::PushStyleColor(ImGuiCol_Text, GetColor(PreLineColor[i]));
                ImGui::TextUnformatted(LineStart, LineEnd);
                ImGui::PopStyleColor();
            }
        }

        Clipper.End();
    }

    // 最新的日志
    if (bAutoScroll &&
        ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}

void FEditorLogSystem::AddLineColor(e_error InColor)
{
    PreLineColor.push_back(InColor);
}

void FEditorLogSystem::ResetLineOffsets(e_error InColor, int InOldSize)
{
    TextBuff.append("\n");

    for (int i = TextBuff.size(); InOldSize < i; InOldSize++)
    {
        if (TextBuff[InOldSize] == '\n')
        {
            LineOffsets.push_back(InOldSize + 1);

            AddLineColor(InColor);
        }
    }
}

ImVec4 FEditorLogSystem::GetColor(e_error InColorID)
{
    switch (InColorID)
    {
    case SIMPLE_C_LOG:
        return ImVec4(1.f, 1.f, 1.f, 1.f);
    case SIMPLE_C_ERROR:
        return ImVec4(1.f, 0.f, 0.f, 1.f);
    case SIMPLE_C_WARNING:
        return ImVec4(1.f, 1.f, 0.f, 1.f);
    case SIMPLE_C_SUCCESS:
        return ImVec4(0.f, 1.f, 0.f, 1.f);
    }

    return ImVec4(0.4f, 0.4f, 0.4f, 1.f);
}
