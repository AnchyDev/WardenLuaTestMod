/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "Warden.h"

std::string welcomePayload = "message('Welcome to the server!');";
uint16 welcomePayloadId;

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

        welcomePayloadId = warden->RegisterPayload(welcomePayload);
        warden->QueuePayload(welcomePayloadId);
    }

    void OnBeforeSendChatMessage(Player* player, uint32& type, uint32& lang, std::string& msg)
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

        std::string payload = Acore::StringFormatFmt("message('{}');", msg);
        uint16 payloadId = warden->RegisterPayload(payload);

        warden->QueuePayload(payloadId);
        warden->ForceChecks();
    }
};

void AddMyScripts()
{
    new WardenLuaPlayer();
}
