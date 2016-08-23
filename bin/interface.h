#pragma once

class IApi
{
    public:

        virtual void Print(const char*) = 0;
};

class IScript
{
    public:

        virtual void SetStateMemorySize(int) = 0;
        virtual bool SaveState(void*, int) = 0;
        virtual bool LoadState() = 0;
};