/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "Warden.h"

std::string welcomePayload = "message('Welcome to the server!');";

class WardenLuaPlayer : public PlayerScript
{
public:
    WardenLuaPlayer() : PlayerScript("WardenLuaPlayer") { }

    void RunTests(WardenPayloadMgr* mgr)
    {
        mgr->ClearQueuedPayloads();

        //Should fail
        ASSERT(!mgr->RegisterPayload("", 4000, false));
        LOG_INFO("module", "Warden unit test #1 passed.");
        //Should pass
        ASSERT(mgr->RegisterPayload("", 9000, false));
        LOG_INFO("module", "Warden unit test #2 passed.");
        //Should fail
        ASSERT(!mgr->RegisterPayload("", 9000, false));
        LOG_INFO("module", "Warden unit test #3 passed.");
        //Should pass
        ASSERT(mgr->RegisterPayload("", 9000, true));
        LOG_INFO("module", "Warden unit test #4 passed.");
        //Should pass
        ASSERT(mgr->UnregisterPayload(9000));
        LOG_INFO("module", "Warden unit test #5 passed.");

        mgr->ClearQueuedPayloads();
    }

    void OnLogin(Player* player) override
    {
        if (!sConfigMgr->GetOption<bool>("WardenLuaTestMod.Enable", false))
        {
            return;
        }

        Warden* warden = player->GetSession()->GetWarden();
        if (!warden)
        {
            return;
        }

        auto payloadMgr = warden->GetPayloadMgr();
        if (!payloadMgr)
        {
            return;
        }

        RunTests(payloadMgr);

        uint32 payloadId = payloadMgr->RegisterPayload(welcomePayload);
        payloadMgr->QueuePayload(payloadId);
    }

    void OnBeforeSendChatMessage(Player* player, uint32& type, uint32& lang, std::string& msg)
    {
        if (!sConfigMgr->GetOption<bool>("WardenLuaTestMod.Enable", false))
        {
            return;
        }

        auto warden = player->GetSession()->GetWarden();
        if (!warden)
        {
            return;
        }

        auto payloadMgr = warden->GetPayloadMgr();
        if (!payloadMgr)
        {
            return;
        }

        std::string payload = Acore::StringFormatFmt("print('{}');", msg);
        uint32 payloadId = payloadMgr->RegisterPayload(payload);
        
        for (uint32 i = 0; i < payloadMgr->GetPayloadCountInQueue(); i++)
        {
            warden->ForceChecks();
        }

        payloadMgr->QueuePayload(payloadId);
        warden->ForceChecks();
    }
};

void AddMyScripts()
{
    new WardenLuaPlayer();
}
