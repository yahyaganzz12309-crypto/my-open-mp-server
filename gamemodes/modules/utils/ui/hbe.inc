new PlayerText: Entry_HUD[MAX_PLAYERS][16];

CreatePlayerTextDraws(playerid)
{
	Entry_HUD[playerid][0] = CreatePlayerTextDraw(playerid, 584.000, 326.000, "LD_SPAC:white");
	PlayerTextDrawTextSize(playerid, Entry_HUD[playerid][0], 57.000, 49.000);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][0], 1);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][0], 190);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][0], 0);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][0], 0);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][0], 255);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][0], 4);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][0], 1);

	Entry_HUD[playerid][1] = CreatePlayerTextDraw(playerid, 558.000, 300.000, "_");
	PlayerTextDrawTextSize(playerid, Entry_HUD[playerid][1], 64.000, 88.000);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][1], 1);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][1], -1);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][1], 0);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][1], 0);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][1], 0);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][1], 5);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][1], 0);
	PlayerTextDrawSetPreviewModel(playerid, Entry_HUD[playerid][1], 2880);
	PlayerTextDrawSetPreviewRot(playerid, Entry_HUD[playerid][1], -17.000, -88.000, 8.000, 3.497);
	PlayerTextDrawSetPreviewVehCol(playerid, Entry_HUD[playerid][1], 0, 0);

	Entry_HUD[playerid][2] = CreatePlayerTextDraw(playerid, 620.000, 335.000, "100");
	PlayerTextDrawLetterSize(playerid, Entry_HUD[playerid][2], 0.218, 1.090);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][2], 2);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][2], -1);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][2], 1);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][2], 1);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][2], 150);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][2], 1);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][2], 1);

	Entry_HUD[playerid][3] = CreatePlayerTextDraw(playerid, 565.000, 317.000, "_");
	PlayerTextDrawTextSize(playerid, Entry_HUD[playerid][3], 64.000, 88.000);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][3], 1);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][3], -1);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][3], 0);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][3], 0);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][3], 0);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][3], 5);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][3], 0);
	PlayerTextDrawSetPreviewModel(playerid, Entry_HUD[playerid][3], 19835);
	PlayerTextDrawSetPreviewRot(playerid, Entry_HUD[playerid][3], -27.000, 0.000, 8.000, 3.697);
	PlayerTextDrawSetPreviewVehCol(playerid, Entry_HUD[playerid][3], 0, 0);

	Entry_HUD[playerid][4] = CreatePlayerTextDraw(playerid, 621.000, 356.000, "100");
	PlayerTextDrawLetterSize(playerid, Entry_HUD[playerid][4], 0.218, 1.090);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][4], 2);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][4], -1);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][4], 1);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][4], 1);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][4], 150);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][4], 1);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][4], 1);

	Entry_HUD[playerid][5] = CreatePlayerTextDraw(playerid, 584.000, 244.000, "LD_SPAC:white");
	PlayerTextDrawTextSize(playerid, Entry_HUD[playerid][5], 57.000, 78.000);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][5], 1);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][5], 190);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][5], 0);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][5], 0);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][5], 255);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][5], 4);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][5], 1);

	Entry_HUD[playerid][6] = CreatePlayerTextDraw(playerid, 562.000, 214.000, "_");
	PlayerTextDrawTextSize(playerid, Entry_HUD[playerid][6], 64.000, 88.000);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][6], 1);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][6], -1);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][6], 0);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][6], 0);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][6], 0);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][6], 5);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][6], 0);
	PlayerTextDrawSetPreviewModel(playerid, Entry_HUD[playerid][6], 19627);
	PlayerTextDrawSetPreviewRot(playerid, Entry_HUD[playerid][6], -42.000, -9.000, -119.000, 2.897);
	PlayerTextDrawSetPreviewVehCol(playerid, Entry_HUD[playerid][6], 0, 0);

	Entry_HUD[playerid][7] = CreatePlayerTextDraw(playerid, 618.000, 252.000, "100");
	PlayerTextDrawLetterSize(playerid, Entry_HUD[playerid][7], 0.218, 1.090);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][7], 2);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][7], -1);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][7], 1);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][7], 1);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][7], 150);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][7], 1);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][7], 1);

	Entry_HUD[playerid][8] = CreatePlayerTextDraw(playerid, 563.000, 236.000, "_");
	PlayerTextDrawTextSize(playerid, Entry_HUD[playerid][8], 64.000, 88.000);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][8], 1);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][8], -1);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][8], 0);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][8], 0);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][8], 0);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][8], 5);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][8], 0);
	PlayerTextDrawSetPreviewModel(playerid, Entry_HUD[playerid][8], 560);
	PlayerTextDrawSetPreviewRot(playerid, Entry_HUD[playerid][8], -42.000, -9.000, -119.000, 3.096);
	PlayerTextDrawSetPreviewVehCol(playerid, Entry_HUD[playerid][8], 0, 0);

	Entry_HUD[playerid][9] = CreatePlayerTextDraw(playerid, 563.000, 270.000, "_");
	PlayerTextDrawTextSize(playerid, Entry_HUD[playerid][9], 64.000, 72.000);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][9], 1);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][9], -1);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][9], 0);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][9], 0);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][9], 0);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][9], 5);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][9], 0);
	PlayerTextDrawSetPreviewModel(playerid, Entry_HUD[playerid][9], 1650);
	PlayerTextDrawSetPreviewRot(playerid, Entry_HUD[playerid][9], -31.000, -11.000, -180.000, 2.996);
	PlayerTextDrawSetPreviewVehCol(playerid, Entry_HUD[playerid][9], 0, 0);

	Entry_HUD[playerid][10] = CreatePlayerTextDraw(playerid, 621.000, 277.000, "100 mph");
	PlayerTextDrawLetterSize(playerid, Entry_HUD[playerid][10], 0.149, 0.998);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][10], 2);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][10], -1);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][10], 1);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][10], 1);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][10], 150);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][10], 1);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][10], 1);

	Entry_HUD[playerid][11] = CreatePlayerTextDraw(playerid, 620.000, 301.000, "100L");
	PlayerTextDrawLetterSize(playerid, Entry_HUD[playerid][11], 0.218, 1.090);
	PlayerTextDrawAlignment(playerid, Entry_HUD[playerid][11], 2);
	PlayerTextDrawColor(playerid, Entry_HUD[playerid][11], -1);
	PlayerTextDrawSetShadow(playerid, Entry_HUD[playerid][11], 1);
	PlayerTextDrawSetOutline(playerid, Entry_HUD[playerid][11], 1);
	PlayerTextDrawBackgroundColor(playerid, Entry_HUD[playerid][11], 150);
	PlayerTextDrawFont(playerid, Entry_HUD[playerid][11], 1);
	PlayerTextDrawSetProportional(playerid, Entry_HUD[playerid][11], 1);
}