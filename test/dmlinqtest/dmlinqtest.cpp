
#include "dmlinq.h"
#include "gtest.h"

class env_dmlinq
{
public:
    void init(){}
    void uninit(){}
};

class frame_dmlinq : public testing::Test
{
public:
    virtual void SetUp()
    {
        env.init();
    }
    virtual void TearDown()
    {
        env.uninit();
    }
protected:
    env_dmlinq env;
};

TEST_F(frame_dmlinq, init)
{
    Idmlinq* module = dmlinqGetModule();
    if (module)
    {
        module->Test();
        module->Release();
    }
}
