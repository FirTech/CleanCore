/*
 * PROJECT:   Senseless Memory Optimization Core
 * FILE:      CleanCore.cpp
 * PURPOSE:   Implementation for Senseless Memory Optimization Core
 */

#include <Windows.h>

#include "MINT.h"

namespace
{
    class FunctionTable
    {
    public:

        FunctionTable();

        NTSTATUS NtSetSystemInformation(
            _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
            _In_opt_ PVOID SystemInformation,
            _In_ ULONG SystemInformationLength);

        NTSTATUS RtlAdjustPrivilege(
            _In_ ULONG Privilege,
            _In_ BOOLEAN Enable,
            _In_ BOOLEAN Client,
            _Out_ PBOOLEAN WasEnabled);

    private:

        HMODULE m_ModuleHandle = nullptr;
        FARPROC m_NtSetSystemInformation = nullptr;
        FARPROC m_RtlAdjustPrivilege = nullptr;
    };

    FunctionTable::FunctionTable()
    {
        this->m_ModuleHandle = ::GetModuleHandleW(L"ntdll.dll");
        if (this->m_ModuleHandle)
        {
            this->m_NtSetSystemInformation = ::GetProcAddress(
                this->m_ModuleHandle,
                "NtSetSystemInformation");
            this->m_RtlAdjustPrivilege = ::GetProcAddress(
                this->m_ModuleHandle,
                "RtlAdjustPrivilege");
        }
    }

    NTSTATUS FunctionTable::NtSetSystemInformation(
        _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
        _In_opt_ PVOID SystemInformation,
        _In_ ULONG SystemInformationLength)
    {
        using ProcType = decltype(::NtSetSystemInformation)*;

        ProcType ProcAddress = reinterpret_cast<ProcType>(
            this->m_NtSetSystemInformation);

        if (!ProcAddress)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        return ProcAddress(
            SystemInformationClass,
            SystemInformation,
            SystemInformationLength);
    }

    NTSTATUS FunctionTable::RtlAdjustPrivilege(
        _In_ ULONG Privilege,
        _In_ BOOLEAN Enable,
        _In_ BOOLEAN Client,
        _Out_ PBOOLEAN WasEnabled)
    {
        using ProcType = decltype(::RtlAdjustPrivilege)*;

        ProcType ProcAddress = reinterpret_cast<ProcType>(
            this->m_RtlAdjustPrivilege);

        if (!ProcAddress)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        return ProcAddress(
            Privilege,
            Enable,
            Client,
            WasEnabled);
    }
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    FunctionTable Functions;

    BOOLEAN WasEnabled;  
    if (!NT_SUCCESS(Functions.RtlAdjustPrivilege(
        SE_PROF_SINGLE_PROCESS_PRIVILEGE,
        TRUE,
        FALSE,
        &WasEnabled)))
    {
        return ERROR_ACCESS_DENIED;
    }

    // Working Sets -> Modified Page List -> Standby List

    SYSTEM_MEMORY_LIST_COMMAND CommandList[] =
    {
        SYSTEM_MEMORY_LIST_COMMAND::MemoryEmptyWorkingSets,
        SYSTEM_MEMORY_LIST_COMMAND::MemoryFlushModifiedList,
        SYSTEM_MEMORY_LIST_COMMAND::MemoryPurgeStandbyList
    };

    for (size_t i = 0; i < sizeof(CommandList) / sizeof(*CommandList); ++i)
    {
        if (!NT_SUCCESS(Functions.NtSetSystemInformation(
            SystemMemoryListInformation,
            &CommandList[i],
            sizeof(SYSTEM_MEMORY_LIST_COMMAND))))
        {
            break;
        }
    }

    return ERROR_SUCCESS;
}
