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
        
        for (uint32 i = 0; i < payloadMgr->GetPayloadsInQueue(); i++)
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
