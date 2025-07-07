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

void FEditorLogSystem::Clear()
{
    TextBuff.clear();
    LineOffsets.clear();
    LineOffsets.push_back(0);
}

void FEditorLogSystem::AddLog(const char* Fmt, ...)
{
    int TextSize = TextBuff.size();

    va_list Args;
    va_start(Args, Fmt);
    TextBuff.appendfv(Fmt, Args);
    va_end(Args);

    TextBuff.append("\n");

    for (int i = TextBuff.size(); TextSize < i; TextSize++)
    {
        if (TextBuff[TextSize] == '\n')
        {
            LineOffsets.push_back(TextSize + 1);
        }
    }
}

void FEditorLogSystem::AddError(const char* Fmt, ...)
{

}

void FEditorLogSystem::AddWarning(const char* Fmt, ...)
{

}

void FEditorLogSystem::AddSuccess(const char* Fmt, ...)
{

}

void FEditorLogSystem::Draw(float DeltaTime)
{
    if (!ImGui::Begin("Log"))
    {
        ImGui::End();
        return;
    }

    {
        if (ImGui::BeginPopup("LogOptions"))
        {
            ImGui::Checkbox("Auto Scroll", &bAutoScroll);
            ImGui::EndPopup();
        }

        ImGui::SameLine();

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

        TextFikter.Draw("LogFilter", -100.0f);
    }

    const char* TextBuffStart = TextBuff.begin();
    const char* TextBuffEnd = TextBuff.end();

    ImGui::Separator();

    ImGui::BeginChild("LogScrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (TextFikter.IsActive())
    {
        for (int i = 0; i < LineOffsets.Size; i++)
        {
            const char* LineStart = TextBuffStart + LineOffsets[i];
            const char* LineEnd = (i + 1 < LineOffsets.Size) ? (TextBuffStart + LineOffsets[i + 1] - 1) : TextBuffEnd;

            if (TextFikter.PassFilter(LineStart, LineEnd))
            {
                ImGui::TextUnformatted(LineStart, LineEnd);
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

                ImGui::TextUnformatted(LineStart, LineEnd);
            }
        }

        Clipper.End();
    }

    if (bAutoScroll &&
        ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}
