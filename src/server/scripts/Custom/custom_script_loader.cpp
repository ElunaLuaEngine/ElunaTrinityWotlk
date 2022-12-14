/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// This is where scripts' loading functions should be declared:
void AddSC_Transmogrification();
void AddSC_npc_1v1arena();
void AddSC_AutoBalance();
void AddSC_CrossfactionBattlegrounds();
void AddLfgSoloScripts();
void Add_paymaster_npc();
void AddSC_racial_traits_npc();
void AddSolocraftScripts();
void AddSC_skip_StarterArea();
void AddSC_AutoLearn();
void AddSC_channel_factions();
void AddSC_Donor_Voter_Trader();
void AddSC_PlayedRewards();
void AddSC_TemplateNPC();

// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddCustomScripts()
{
    AddSC_Transmogrification();
    AddSC_npc_1v1arena();
    AddSC_AutoBalance();
    AddSC_CrossfactionBattlegrounds();
    AddLfgSoloScripts();
    Add_paymaster_npc();
    AddSC_racial_traits_npc();
    AddSolocraftScripts();
    AddSC_skip_StarterArea();
    AddSC_AutoLearn();
    AddSC_channel_factions();
    AddSC_Donor_Voter_Trader();
    AddSC_PlayedRewards();
    AddSC_TemplateNPC();
}
