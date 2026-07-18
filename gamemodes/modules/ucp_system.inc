/*
    ============================================================
    UCP_SYSTEM.INC - Sistem Register & Login (UCP) + Verifikasi
    ============================================================
    Di-include dari main.pwn lewat #include "ucp_system.inc"
    Semua fungsi & variabel di sini pakai awalan "UCP_" / "UCPPlayer"
    biar gak bentrok kalau nanti kamu tambah file .inc lain.

    Alur: connect -> cek nama in-game di database
      -> sudah terdaftar -> Login (password)
      -> belum terdaftar -> Daftar: kontak (email/Discord) -> kode
         verifikasi -> buat password -> konfirmasi password

    CARA KIRIM KODE VERIFIKASI: game server manggil langsung ke
    sebuah Cloudflare Worker (relay-worker/) lewat HTTP POST tiap
    ada kode yang perlu dikirim. Worker itu yang beneran ngirim
    lewat Discord DM / Email. Ini cocok buat Cloudflare Workers
    karena dia punya alamat publik permanen begitu di-deploy, jadi
    bisa dipanggil dari mana aja termasuk dari LeMeHost.

    WAJIB DIISI DULU (cari kata "GANTI"):
    - MYSQL_HOST / MYSQL_USER / MYSQL_PASSWORD / MYSQL_DATABASE
    - UCP_NOTIFY_URL / UCP_NOTIFY_SECRET (alamat Worker-nya, isi
      SETELAH kamu deploy relay-worker - lihat instruksinya)

    WAJIB TERPASANG SEBELUM DI-COMPILE:
    - Plugin MySQL pBlueG/SA-MP-MySQL rilis R41-4
      github.com/pBlueG/SA-MP-MySQL/releases/tag/R41-4

    CATATAN SOAL HTTPS: alamat Cloudflare Worker itu HTTPS
    (bukan HTTP biasa). Native HTTP() bawaan open.mp SEHARUSNYA
    bisa, tapi kalau kode gak nyampe padahal Worker-nya sudah aktif
    (cek log Worker-nya di dashboard Cloudflare), kemungkinan besar
    itu sebabnya - kabari aku, nanti kita ganti pakai plugin
    pawn-requests yang HTTPS-nya lebih terjamin.

    CATATAN MIGRASI: kalau kamu udah pernah nyoba versi sebelumnya
    (tabel lama `pyl_accounts`), skemanya beda (belum ada kolom
    contact_type/contact_value). Hapus dulu tabel lamanya lewat
    MySQL client kamu (DROP TABLE pyl_accounts;) biar tabel baru
    `ucp_accounts` yang lengkap otomatis kebuat pas server nyala.
*/

#if defined _inc_ucp_system
    #endinput
#endif
#define _inc_ucp_system

#include <a_mysql>

// ================== KONFIGURASI MYSQL (GANTI!) ==================
#define MYSQL_HOST        "bfufgwep3pmfqugddhh0-mysql.services.clever-cloud.com"
#define MYSQL_USER        "urboygohlzlv7upa"
#define MYSQL_PASSWORD    "IL5nk6lzSEieTVaRlqNC"
#define MYSQL_DATABASE    "bfufgwep3pmfqugddhh0"

// ============== KONFIGURASI RELAY NOTIFIKASI (GANTI!) ==============
// Isi UCP_NOTIFY_URL setelah kamu deploy relay-worker (langkah di akhir
// panduan). Bentuknya kira-kira: "nama-worker.akun-kamu.workers.dev/send-code"
#define UCP_NOTIFY_URL "https://pylance-bot-discord.yahyanursyafaady.workers.dev/send-code"
#define UCP_NOTIFY_SECRET    "aghyrdguu65543fhi9965rrfgjko096321wfcvbu6rrgkkoo00ihgszvbnnj"

// ================== KONFIGURASI UMUM ==================
#undef MAX_PLAYERS
#define MAX_PLAYERS 50  // server buat komunitas kecil, hemat memori (samakan sama config.json)

#define UCP_LOGIN_TIME_LIMIT     120000  // 120 detik nyelesein daftar/login (ada langkah verifikasi jadi lebih lama)
#define UCP_MAX_LOGIN_ATTEMPTS   3
#define UCP_MAX_VERIFY_ATTEMPTS  5
#define UCP_MAX_RESEND          3
#define UCP_CODE_EXPIRY_MS       300000  // kode basi setelah 5 menit
#define UCP_PASSWORD_MIN         6
#define UCP_PASSWORD_MAX         16

#define UCP_CONTACT_EMAIL     1
#define UCP_CONTACT_DISCORD   2

#define COLOR_RED      0xFF4444FF
#define COLOR_GREEN    0x44DD44FF
#define COLOR_YELLOW   0xFFD700FF

// musik yang diputar selama proses daftar/login (dipilih random)
new const g_MusicURLs[][] = {
    "https://b.top4top.io/m_3846nqj8i1.mp3",
    "https://c.top4top.io/m_3846obamw2.mp3",
    "https://d.top4top.io/m_3846j0waj3.mp3"
};

// titik spawn default setelah berhasil daftar/login (Grove Street, Los Santos)
#define SPAWN_POS_X    2495.3547
#define SPAWN_POS_Y    -1688.2319
#define SPAWN_POS_Z    13.6774
#define SPAWN_ANGLE    351.1646

// ================== ID DIALOG ==================
enum
{
    DIALOG_UCP_CONTACT = 30000,   // input email / Discord User ID
    DIALOG_UCP_VERIFY,            // input kode verifikasi
    DIALOG_UCP_REGISTER,          // buat password baru
    DIALOG_UCP_REGISTER_CONFIRM,  // konfirmasi ulang password
    DIALOG_UCP_LOGIN              // masukin password buat login
};

// ================== DATA PER PLAYER ==================
enum E_UCP_PLAYER
{
    bool:LoggedIn,
    ContactType,                          // UCP_CONTACT_EMAIL atau UCP_CONTACT_DISCORD
    ContactValue[65],                     // isi email atau Discord User ID-nya
    VerifyCode[7],                        // kode verifikasi 6 digit yang lagi aktif
    VerifyCodeTime,                       // GetTickCount() pas kode dibuat, buat cek kadaluarsa
    VerifyAttempts,
    ResendCount,
    TempPassword[UCP_PASSWORD_MAX + 1],   // password baru sebelum dikonfirmasi
    StoredHash[65],
    StoredSalt[17],
    LoginAttempts,
    LoginTimer,
    RaceCheck                             // penanda sesi koneksi, jaga-jaga race condition query MySQL
};
new UCPPlayer[MAX_PLAYERS][E_UCP_PLAYER];

// handle koneksi ke database MySQL
new MySQL:g_SQL;

// ================== FORWARD DECLARATION ==================
forward UCP_LoginTimeout(playerid);
forward UCP_OnNotifySent(index, response_code, responseData[]);
forward UCP_OnAccountChecked(playerid, race_check);
forward UCP_OnContactChecked(playerid, race_check);
forward UCP_OnRegistrationSaved(playerid, race_check);


// ============================================================
// Fungsi bantu - sengaja ditaruh di atas (sebelum dipakai) supaya
// urutan kompilasinya jelas dan aman
// ============================================================

UCP_GenerateSalt(output[])
{
    new const charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    for (new i = 0; i < 16; i++) output[i] = charset[random(62)];
    output[16] = '\0';
}

// Membuat kode verifikasi 6 digit baru dan mencatat jam pembuatannya
UCP_GenerateCode(playerid)
{
    new code = random(1000000);
    format(UCPPlayer[playerid][VerifyCode], 7, "%06d", code);
    UCPPlayer[playerid][VerifyCodeTime] = GetTickCount();
}

// Mengecek format email secara sederhana (cuma cek bentuknya, bukan lewat internet)
bool:UCP_IsValidEmail(const email[])
{
    new len = strlen(email);
    if (len < 5 || len > 64) return false;

    new at_pos = strfind(email, "@", false);
    if (at_pos < 1) return false;
    if (strfind(email, "@", false, at_pos + 1) != -1) return false; // lebih dari 1 tanda @

    new dot_pos = strfind(email, ".", false, at_pos);
    if (dot_pos == -1) return false;
    if (dot_pos >= len - 2) return false;

    if (strfind(email, " ", false) != -1) return false;

    return true;
}

// Mengecek format Discord User ID (angka semua, 15-20 digit - Discord ID
// asli sekarang biasanya 17-19 digit, dikasih sedikit buffer)
bool:UCP_IsValidDiscordID(const id[])
{
    new len = strlen(id);
    if (len < 15 || len > 20) return false;

    for (new i = 0; i < len; i++)
    {
        if (id[i] < '0' || id[i] > '9') return false;
    }
    return true;
}


// Memutar 1 dari 3 link musik secara acak
UCP_PlayRandomMusic(playerid)
{
    new idx = random(sizeof(g_MusicURLs));
    PlayAudioStreamForPlayer(playerid, g_MusicURLs[idx]);
}

// Meng-encode teks biar aman ditaruh di URL/form data (karakter kayak
// '+', '&', '=' bisa merusak parsing di sisi relay kalau gak di-encode -
// ini penting soalnya email boleh mengandung '+', misalnya nama+test@gmail.com)
UCP_UrlEncode(dest[], const src[], maxlen)
{
    new const hexChars[] = "0123456789ABCDEF";
    new destIdx = 0;
    new len = strlen(src);

    for (new i = 0; i < len && destIdx < maxlen - 4; i++)
    {
        new c = src[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~')
        {
            dest[destIdx++] = c;
        }
        else
        {
            dest[destIdx++] = '%';
            dest[destIdx++] = hexChars[(c >> 4) & 0x0F];
            dest[destIdx++] = hexChars[c & 0x0F];
        }
    }
    dest[destIdx] = '\0';
}

// Meminta Worker mengirim kode verifikasi lewat Discord DM / Email.
UCP_SendVerificationCode(playerid)
{
    new channel[8];
    if (UCPPlayer[playerid][ContactType] == UCP_CONTACT_EMAIL) format(channel, sizeof(channel), "email");
    else format(channel, sizeof(channel), "discord");

    new encodedContact[196];
    UCP_UrlEncode(encodedContact, UCPPlayer[playerid][ContactValue], sizeof(encodedContact));

    new data[256];
    format(data, sizeof(data), "secret=%s&channel=%s&destination=%s&code=%s",
        UCP_NOTIFY_SECRET, channel, encodedContact, UCPPlayer[playerid][VerifyCode]);

    HTTP(playerid, HTTP_POST, UCP_NOTIFY_URL, data, "UCP_OnNotifySent");

    // buat jaga-jaga / debug, kode juga dicetak di console
    printf("[UCP VERIFY] Kode untuk '%s' (%s): %s", UCPPlayer[playerid][ContactValue], channel, UCPPlayer[playerid][VerifyCode]);
}

public UCP_OnNotifySent(index, response_code, responseData[])
{
    #pragma unused responseData
    if (response_code != 200 && IsPlayerConnected(index))
    {
        SendClientMessage(index, COLOR_YELLOW, "Relay belum bisa dihubungi - untuk sementara kode tercetak di console server.");
    }
    return 1;
}

// Dipanggil begitu daftar/login sukses: buka kunci gerakan, matikan musik
// & timer, lalu spawn-kan player ke titik default
UCP_FinishAuth(playerid)
{
    UCPPlayer[playerid][LoggedIn] = true;

    TogglePlayerControllable(playerid, true);
    StopAudioStreamForPlayer(playerid);

    if (UCPPlayer[playerid][LoginTimer])
    {
        KillTimer(UCPPlayer[playerid][LoginTimer]);
        UCPPlayer[playerid][LoginTimer] = 0;
    }

    SetSpawnInfo(playerid, 0, 0, SPAWN_POS_X, SPAWN_POS_Y, SPAWN_POS_Z, SPAWN_ANGLE);
    SpawnPlayer(playerid);
}

UCP_ShowContact(playerid, const errorMsg[])
{
    new body[300];
    if (strlen(errorMsg) > 0) format(body, sizeof(body), "{FF4444}%s{FFFFFF}\n\nMasukkan EMAIL atau DISCORD USER ID kamu:", errorMsg);
    else format(body, sizeof(body), "Kamu belum terdaftar.\nMasukkan EMAIL atau DISCORD USER ID kamu untuk verifikasi.\n\n{AAAAAA}Cara lihat Discord User ID: Discord > Settings > Advanced > nyalain 'Developer Mode', lalu klik kanan foto profil kamu > 'Copy User ID'.{FFFFFF}");
    ShowPlayerDialog(playerid, DIALOG_UCP_CONTACT, DIALOG_STYLE_INPUT, "Daftar Akun - Langkah 1/3", body, "Lanjut", "Keluar");
}

UCP_ShowVerify(playerid, const errorMsg[])
{
    new channelName[16];
    if (UCPPlayer[playerid][ContactType] == UCP_CONTACT_EMAIL) format(channelName, sizeof(channelName), "email");
    else format(channelName, sizeof(channelName), "Discord");

    new body[256];
    if (strlen(errorMsg) > 0) format(body, sizeof(body), "{FF4444}%s{FFFFFF}\n\nMasukkan kode 6 digit yang dikirim lewat %s:", errorMsg, channelName);
    else format(body, sizeof(body), "Kode verifikasi 6 digit sudah dikirim lewat %s.\nMasukkan kodenya di sini:", channelName);

    ShowPlayerDialog(playerid, DIALOG_UCP_VERIFY, DIALOG_STYLE_INPUT, "Daftar Akun - Langkah 2/3", body, "Kirim", "Kirim Ulang Kode");
}

UCP_ShowRegister(playerid, const errorMsg[])
{
    new body[256];
    if (strlen(errorMsg) > 0) format(body, sizeof(body), "{FF4444}%s{FFFFFF}\n\nBuat password baru (%d-%d karakter):", errorMsg, UCP_PASSWORD_MIN, UCP_PASSWORD_MAX);
    else format(body, sizeof(body), "Kode terverifikasi!\nSekarang buat password kamu (%d-%d karakter):", UCP_PASSWORD_MIN, UCP_PASSWORD_MAX);
    ShowPlayerDialog(playerid, DIALOG_UCP_REGISTER, DIALOG_STYLE_PASSWORD, "Daftar Akun - Langkah 3/3", body, "Daftar", "Keluar");
}

UCP_ShowRegisterConfirm(playerid, const errorMsg[])
{
    new body[256];
    if (strlen(errorMsg) > 0) format(body, sizeof(body), "{FF4444}%s{FFFFFF}\n\nMasukkan ulang password yang sama untuk konfirmasi:", errorMsg);
    else format(body, sizeof(body), "Masukkan ulang password yang sama untuk konfirmasi:");
    ShowPlayerDialog(playerid, DIALOG_UCP_REGISTER_CONFIRM, DIALOG_STYLE_PASSWORD, "Konfirmasi Password", body, "Konfirmasi", "Keluar");
}

UCP_ShowLogin(playerid, const errorMsg[])
{
    new body[256];
    if (strlen(errorMsg) > 0) format(body, sizeof(body), "{FF4444}%s{FFFFFF}\n\nMasukkan password kamu untuk login:", errorMsg);
    else format(body, sizeof(body), "Akun ini sudah terdaftar.\nMasukkan password kamu untuk login:");
    ShowPlayerDialog(playerid, DIALOG_UCP_LOGIN, DIALOG_STYLE_PASSWORD, "Login Akun", body, "Login", "Keluar");
}


// ============================================================
// Fungsi yang dipanggil dari main.pwn
// ============================================================

UCP_Init()
{
    new MySQLOpt:option_id = mysql_init_options();
    mysql_set_option(option_id, AUTO_RECONNECT, true);

    g_SQL = mysql_connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASSWORD, MYSQL_DATABASE, option_id);

    if (g_SQL == MYSQL_INVALID_HANDLE || mysql_errno(g_SQL) != 0)
    {
        print("[UCP] GAGAL connect ke MySQL. Cek MYSQL_HOST/USER/PASSWORD/DATABASE di ucp_system.inc.");
        SendRconCommand("exit");
        return;
    }
    print("[UCP] Berhasil connect ke MySQL.");

    mysql_tquery(g_SQL, "CREATE TABLE IF NOT EXISTS `ucp_accounts` (`id` INT(11) NOT NULL AUTO_INCREMENT, `username` VARCHAR(24) NOT NULL, `contact_type` TINYINT(1) NOT NULL, `contact_value` VARCHAR(64) NOT NULL, `password_hash` CHAR(64) NOT NULL, `password_salt` CHAR(16) NOT NULL, `register_date` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY (`id`), UNIQUE KEY `username` (`username`), UNIQUE KEY `contact_value` (`contact_value`))");

    // open.mp tetap butuh minimal 1 "class" terdaftar biar sistem spawn-nya
    // gak protes, walaupun player kita gak pernah lewat layar pilih skin biasa
    AddPlayerClass(0, SPAWN_POS_X, SPAWN_POS_Y, SPAWN_POS_Z, SPAWN_ANGLE, 0, 0, 0, 0, 0, 0);
}

UCP_Exit()
{
    mysql_close(g_SQL);
}

UCP_OnPlayerConnect(playerid)
{
    // kosongkan semua data slot ini (jaga-jaga bekas dipakai player lain sebelumnya)
    static const empty_data[E_UCP_PLAYER];
    UCPPlayer[playerid] = empty_data;

    UCPPlayer[playerid][RaceCheck]++;

    TogglePlayerControllable(playerid, false); // kunci gerakan sampai daftar/login selesai
    UCP_PlayRandomMusic(playerid);

    UCPPlayer[playerid][LoginTimer] = SetTimerEx("UCP_LoginTimeout", UCP_LOGIN_TIME_LIMIT, false, "d", playerid);

    // cek ke database: nama in-game ini sudah pernah daftar belum
    new name[MAX_PLAYER_NAME], query[150];
    GetPlayerName(playerid, name, sizeof(name));
    mysql_format(g_SQL, query, sizeof(query), "SELECT * FROM `ucp_accounts` WHERE `username` = '%e' LIMIT 1", name);
    mysql_tquery(g_SQL, query, "UCP_OnAccountChecked", "dd", playerid, UCPPlayer[playerid][RaceCheck]);
}

UCP_OnPlayerDisconnect(playerid, reason)
{
    #pragma unused reason
    UCPPlayer[playerid][RaceCheck]++;

    if (UCPPlayer[playerid][LoginTimer])
    {
        KillTimer(UCPPlayer[playerid][LoginTimer]);
        UCPPlayer[playerid][LoginTimer] = 0;
    }
}

// Hasil pengecekan akun dari UCP_OnPlayerConnect: nentuin munculin
// dialog kontak (buat daftar) atau langsung dialog Login
public UCP_OnAccountChecked(playerid, race_check)
{
    if (race_check != UCPPlayer[playerid][RaceCheck]) return 1;

    if (cache_num_rows() > 0)
    {
        cache_get_value_name(0, "password_hash", UCPPlayer[playerid][StoredHash], 65);
        cache_get_value_name(0, "password_salt", UCPPlayer[playerid][StoredSalt], 17);
        UCP_ShowLogin(playerid, "");
    }
    else
    {
        UCP_ShowContact(playerid, "");
    }
    return 1;
}

// Dipanggil dari OnDialogResponse di main.pwn. Balikin true kalau dialogid
// ini memang punya sistem UCP (dan sudah ditangani di sini), false kalau
// bukan (biar main.pwn bisa lempar ke sistem lain kalau nanti ada).
bool:UCP_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    #pragma unused listitem
    switch (dialogid)
    {
        // -------- Daftar, langkah 1: email atau Discord User ID --------
        case DIALOG_UCP_CONTACT:
        {
            if (!response) { Kick(playerid); return true; }

            if (strfind(inputtext, "@", false) != -1)
            {
                if (!UCP_IsValidEmail(inputtext))
                {
                    UCP_ShowContact(playerid, "Format email tidak valid. Contoh: nama@email.com");
                    return true;
                }
                UCPPlayer[playerid][ContactType] = UCP_CONTACT_EMAIL;
                format(UCPPlayer[playerid][ContactValue], 65, "%s", inputtext);
            }
            else
            {
                if (!UCP_IsValidDiscordID(inputtext))
                {
                    UCP_ShowContact(playerid, "Discord User ID tidak valid (harus angka semua, 15-20 digit).");
                    return true;
                }
                UCPPlayer[playerid][ContactType] = UCP_CONTACT_DISCORD;
                format(UCPPlayer[playerid][ContactValue], 65, "%s", inputtext);
            }

            new query[150];
            mysql_format(g_SQL, query, sizeof(query), "SELECT `id` FROM `ucp_accounts` WHERE `contact_value` = '%e' LIMIT 1", UCPPlayer[playerid][ContactValue]);
            mysql_tquery(g_SQL, query, "UCP_OnContactChecked", "dd", playerid, UCPPlayer[playerid][RaceCheck]);
            return true;
        }

        // -------- Daftar, langkah 2: kode verifikasi --------
        case DIALOG_UCP_VERIFY:
        {
            if (!response) // tombol kanan = "Kirim Ulang Kode"
            {
                if (UCPPlayer[playerid][ResendCount] >= UCP_MAX_RESEND)
                {
                    UCP_ShowVerify(playerid, "Sudah mencapai batas kirim ulang. Hubungi admin.");
                    return true;
                }
                UCPPlayer[playerid][ResendCount]++;
                UCP_GenerateCode(playerid);
                UCP_SendVerificationCode(playerid);
                UCP_ShowVerify(playerid, "Kode baru sudah dikirim.");
                return true;
            }

            if (GetTickCount() - UCPPlayer[playerid][VerifyCodeTime] > UCP_CODE_EXPIRY_MS)
            {
                UCP_ShowVerify(playerid, "Kode sudah kadaluarsa. Klik 'Kirim Ulang Kode' untuk minta kode baru.");
                return true;
            }

            if (!strcmp(inputtext, UCPPlayer[playerid][VerifyCode]))
            {
                UCP_ShowRegister(playerid, "");
            }
            else
            {
                UCPPlayer[playerid][VerifyAttempts]++;
                if (UCPPlayer[playerid][VerifyAttempts] >= UCP_MAX_VERIFY_ATTEMPTS)
                {
                    SendClientMessage(playerid, COLOR_RED, "Terlalu banyak kode yang salah. Kamu dikeluarkan dari server.");
                    Kick(playerid);
                }
                else
                {
                    UCP_ShowVerify(playerid, "Kode salah, coba lagi.");
                }
            }
            return true;
        }

        // -------- Daftar, langkah 3: buat password --------
        case DIALOG_UCP_REGISTER:
        {
            if (!response) { Kick(playerid); return true; }

            new len = strlen(inputtext);
            if (len < UCP_PASSWORD_MIN || len > UCP_PASSWORD_MAX)
            {
                UCP_ShowRegister(playerid, "Panjang password harus 6-16 karakter.");
                return true;
            }

            format(UCPPlayer[playerid][TempPassword], UCP_PASSWORD_MAX + 1, "%s", inputtext);
            UCP_ShowRegisterConfirm(playerid, "");
            return true;
        }

        // -------- Daftar, langkah 4: konfirmasi password --------
        case DIALOG_UCP_REGISTER_CONFIRM:
        {
            if (!response) { Kick(playerid); return true; }

            if (strcmp(inputtext, UCPPlayer[playerid][TempPassword]) != 0)
            {
                UCP_ShowRegisterConfirm(playerid, "Password tidak cocok, coba lagi.");
                return true;
            }

            // password cocok -> hash pakai salt acak, lalu simpan ke database
            new salt[17];
            UCP_GenerateSalt(salt);

            new hash[65];
            SHA256_PassHash(UCPPlayer[playerid][TempPassword], salt, hash, 65);

            new name[MAX_PLAYER_NAME], query[400];
            GetPlayerName(playerid, name, sizeof(name));
            mysql_format(g_SQL, query, sizeof(query),
                "INSERT INTO `ucp_accounts` (`username`, `contact_type`, `contact_value`, `password_hash`, `password_salt`) VALUES ('%e', %d, '%e', '%e', '%e')",
                name, UCPPlayer[playerid][ContactType], UCPPlayer[playerid][ContactValue], hash, salt);
            mysql_tquery(g_SQL, query, "UCP_OnRegistrationSaved", "dd", playerid, UCPPlayer[playerid][RaceCheck]);
            return true;
        }

        // -------- Login: masukin password --------
        case DIALOG_UCP_LOGIN:
        {
            if (!response) { Kick(playerid); return true; }

            new hash[65];
            SHA256_PassHash(inputtext, UCPPlayer[playerid][StoredSalt], hash, 65);

            if (!strcmp(hash, UCPPlayer[playerid][StoredHash]))
            {
                UCP_FinishAuth(playerid);
                SendClientMessage(playerid, COLOR_GREEN, "Login berhasil! Selamat datang kembali.");
            }
            else
            {
                UCPPlayer[playerid][LoginAttempts]++;
                if (UCPPlayer[playerid][LoginAttempts] >= UCP_MAX_LOGIN_ATTEMPTS)
                {
                    SendClientMessage(playerid, COLOR_RED, "Password salah 3 kali. Kamu dikeluarkan dari server.");
                    Kick(playerid);
                }
                else
                {
                    new sisa = UCP_MAX_LOGIN_ATTEMPTS - UCPPlayer[playerid][LoginAttempts];
                    new msg[64];
                    format(msg, sizeof(msg), "Password salah! Sisa percobaan: %d.", sisa);
                    UCP_ShowLogin(playerid, msg);
                }
            }
            return true;
        }
    }
    return false;
}

// Hasil pengecekan kontak (email/Discord) dari langkah 1 daftar
public UCP_OnContactChecked(playerid, race_check)
{
    if (race_check != UCPPlayer[playerid][RaceCheck]) return 1;

    if (cache_num_rows() > 0)
    {
        UCP_ShowContact(playerid, "Email/Discord User ID ini sudah terdaftar.");
    }
    else
    {
        UCPPlayer[playerid][VerifyAttempts] = 0;
        UCPPlayer[playerid][ResendCount] = 0;
        UCP_GenerateCode(playerid);
        UCP_SendVerificationCode(playerid);
        UCP_ShowVerify(playerid, "");
    }
    return 1;
}

// Hasil penyimpanan akun baru ke database
public UCP_OnRegistrationSaved(playerid, race_check)
{
    if (race_check != UCPPlayer[playerid][RaceCheck]) return 1;

    if (mysql_errno(g_SQL) != 0)
    {
        // kemungkinan kecil: ada yang keduluan pakai nama/kontak yang sama pas proses ini jalan
        SendClientMessage(playerid, COLOR_RED, "Gagal menyimpan akun, coba connect ulang.");
        Kick(playerid);
        return 1;
    }

    UCPPlayer[playerid][TempPassword][0] = '\0';

    UCP_FinishAuth(playerid);
    SendClientMessage(playerid, COLOR_GREEN, "Akun berhasil dibuat! Selamat datang di Pylance.");
    return 1;
}

public UCP_LoginTimeout(playerid)
{
    UCPPlayer[playerid][LoginTimer] = 0;
    if (IsPlayerConnected(playerid) && !UCPPlayer[playerid][LoggedIn])
    {
        SendClientMessage(playerid, COLOR_RED, "Waktu untuk daftar/login sudah habis.");
        Kick(playerid);
    }
    return 1;
}
