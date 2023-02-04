#pragma once

class BaseGen {
public:
    virtual void run() = 0;
    virtual ~BaseGen() = default;
};
