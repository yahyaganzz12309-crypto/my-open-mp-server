/*
    ============================================================
    Pylance - main.pwn
    ============================================================
    File utama gamemode. Logic daftar/login/verifikasi ada di
    ucp_system.inc - file ini cuma "menyambungkan" callback open.mp
    ke fungsi-fungsi di sana. Kalau nanti nambah fitur lain, tinggal
    bikin file .inc baru dan sambungkan dengan cara yang sama.
*/

#include <open.mp>
#include "modules/ucp_system.inc"
#include "modules/utils/ui/hbe.inc"

main()
{
    print("\n----------------------------------------");
    print(" Pylance aktif");
    print("----------------------------------------\n");
}

public OnGameModeInit()
{
    UCP_Init();
    return 1;
}

public OnGameModeExit()
{
    UCP_Exit();
    return 1;
}

public OnPlayerConnect(playerid)
{
    UCP_OnPlayerConnect(playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    UCP_OnPlayerDisconnect(playerid, reason);
    return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    // kalau nanti nambah file .inc lain yang juga punya dialog sendiri,
    // tinggal tambah pemanggilan serupa di sini
    if (UCP_OnDialogResponse(playerid, dialogid, response, listitem, inputtext)) return 1;
    return 0;
}

// Kerangka minimal spawn (player kita gak pernah lewat sini secara normal
// karena langsung di-spawn dari UCP_FinishAuth, tapi tetap disediakan
// biar open.mp tidak error kalau ke-trigger)
public OnPlayerRequestClass(playerid)
{
    SetPlayerPos(playerid, SPAWN_POS_X, SPAWN_POS_Y, SPAWN_POS_Z);
    SetPlayerCameraPos(playerid, SPAWN_POS_X, SPAWN_POS_Y - 6.0, SPAWN_POS_Z);
    SetPlayerCameraLookAt(playerid, SPAWN_POS_X, SPAWN_POS_Y, SPAWN_POS_Z);
    return 1;
}

public OnPlayerSpawn(playerid)
{
    #pragma unused playerid
    return 1;
}
