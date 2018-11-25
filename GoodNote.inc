/*

	Author: knox
	Version: 1.2
	Data: 24.11.2018

*/

#if defined _GoodNote_included
	#endinput
#endif

#define _GoodNote_included

#if !defined MAX_GOOD_NOTES
	#define MAX_GOOD_NOTES 5
#endif

#if !defined MAX_GOOD_TEXT_LENGTH
	#define MAX_GOOD_TEXT_LENGTH 65
#endif

#define GN_DIRECTION_UP 1		/* /\ */
#define GN_DIRECTION_DOWN 2		/* \/ */

enum GOOD_NOTE_INFO
{
	GOOD_NOTE_ID,
	PlayerText:GOOD_NOTE_TD_BG,
	PlayerText:GOOD_NOTE_TD_TEXT,
	GOOD_NOTE_TEXT[MAX_GOOD_TEXT_LENGTH],
	GOOD_NOTE_TIME_OUT,
	GOOD_NOTE_TIMER,
	GOOD_NOTE_LINES,
	GOOD_NOTE_COL_BG,
	GOOD_NOTE_COL_TEXT
}

static
	GoodNoteData[MAX_PLAYERS][MAX_GOOD_NOTES][GOOD_NOTE_INFO],
	GoodNoteLastId = 0,
	Float:GoodNotePosX = 497.0,
	Float:GoodNotePosY = 100.0,
	Float:GoodNoteWidth = 111.0,
	GoodNoteDirection = GN_DIRECTION_DOWN,
	GoodNoteSound = 1149
;

stock SetGoodNoteSound(sound)
{
	GoodNoteSound = sound;
	return 1;
}

stock SetGoodNoteDirection(type)
{
	switch(type)
	{
		case GN_DIRECTION_UP, GN_DIRECTION_DOWN:
		{
			GoodNoteDirection = type;
			return 1;
		}
	}
	return 0;
}

stock SetGoodNoteWidth(Float:width)
{
	GoodNoteWidth = width;
	return 1;
}

stock SetGoodNotePos(Float:pos_x, Float:pos_y)
{
	GoodNotePosX = pos_x;
	GoodNotePosY = pos_y;
	return 1;
}

stock CreateGoodNote(playerid, const text[], time = 0, color = -1, bg_color = 255, sound = 1, const size = sizeof(text))
{
	static
		free_pos,
		lines_count,
		Float:size_bg,
		Float:pos_y
	;
	free_pos = gn_GetFreeNotePos(playerid);
	if(free_pos == -1) return 0;

	lines_count = gn_GetLineOfString(text, size);

	size_bg = 1.2 + (lines_count * 1.6);

	pos_y = GoodNotePosY;

	for(new i; i < free_pos; i++)
	{
		if(GoodNoteDirection == GN_DIRECTION_DOWN) {
			pos_y += 12.0 + (GoodNoteData[playerid][i][GOOD_NOTE_LINES] * 16.0) + 5.0;
		} else {
			pos_y -= 12.0 + (GoodNoteData[playerid][i][GOOD_NOTE_LINES] * 16.0) + 5.0;
		}
	}

	if(lines_count > 1) {
		pos_y -= (lines_count - 1) * 16.0;
	}

	GoodNoteData[playerid][free_pos][GOOD_NOTE_LINES] = lines_count;
	GoodNoteData[playerid][free_pos][GOOD_NOTE_COL_BG] = bg_color;
	GoodNoteData[playerid][free_pos][GOOD_NOTE_COL_TEXT] = color;

	new PlayerText:temp_td = CreatePlayerTextDraw(playerid, GoodNotePosX, pos_y, "Box");
	PlayerTextDrawLetterSize(playerid, temp_td, 0.0, size_bg);
	PlayerTextDrawTextSize(playerid, temp_td, GoodNotePosX + GoodNoteWidth, 0.0);
	PlayerTextDrawUseBox(playerid, temp_td, 1);
	PlayerTextDrawBoxColor(playerid, temp_td, bg_color);
	PlayerTextDrawShow(playerid, temp_td);

	GoodNoteData[playerid][free_pos][GOOD_NOTE_TD_BG] = temp_td;

	format(GoodNoteData[playerid][free_pos][GOOD_NOTE_TEXT], MAX_GOOD_TEXT_LENGTH, text);

	temp_td = CreatePlayerTextDraw(playerid, GoodNotePosX + 1.5, pos_y + 6.0, GoodNoteData[playerid][free_pos][GOOD_NOTE_TEXT]);
	PlayerTextDrawLetterSize(playerid, temp_td, 0.4, 1.6);
	PlayerTextDrawTextSize(playerid, temp_td, 500.3332, 0.0);
	PlayerTextDrawColor(playerid, temp_td, color);
	PlayerTextDrawFont(playerid, temp_td, 1);
	PlayerTextDrawSetShadow(playerid, temp_td, 0);
	PlayerTextDrawShow(playerid, temp_td);

	GoodNoteLastId++;

	GoodNoteData[playerid][free_pos][GOOD_NOTE_TD_TEXT] = temp_td;

	if(time == 0) {
		GoodNoteData[playerid][free_pos][GOOD_NOTE_TIME_OUT] = 0;
		GoodNoteData[playerid][free_pos][GOOD_NOTE_TIMER] = 0;
	} else {
		GoodNoteData[playerid][free_pos][GOOD_NOTE_TIME_OUT] = time + gettime();
		GoodNoteData[playerid][free_pos][GOOD_NOTE_TIMER] = SetTimerEx("DestroyGoodNote", time * 1000, false, "dd", playerid, GoodNoteLastId);
	}

	if(sound) {
		PlayerPlaySound(playerid, GoodNoteSound, 0.0, 0.0, 0.0);
	}

	GoodNoteData[playerid][free_pos][GOOD_NOTE_ID] = GoodNoteLastId;
	return GoodNoteLastId;
}

forward DestroyGoodNote(playerid, goodnoteid);
public DestroyGoodNote(playerid, goodnoteid)
{
	static pos;
	pos = gn_FindGoodNotePosById(playerid, goodnoteid);
	if(pos == -1) return 0;
	if(GoodNoteData[playerid][pos][GOOD_NOTE_TIMER] != 0) {
		KillTimer(GoodNoteData[playerid][pos][GOOD_NOTE_TIMER]);
	}
	PlayerTextDrawHide(playerid, GoodNoteData[playerid][pos][GOOD_NOTE_TD_BG]);
	PlayerTextDrawDestroy(playerid, GoodNoteData[playerid][pos][GOOD_NOTE_TD_BG]);
	PlayerTextDrawHide(playerid, GoodNoteData[playerid][pos][GOOD_NOTE_TD_TEXT]);
	PlayerTextDrawDestroy(playerid, GoodNoteData[playerid][pos][GOOD_NOTE_TD_TEXT]);
	GoodNoteData[playerid][pos][GOOD_NOTE_ID] = 0;

	if(pos + 1 >= MAX_GOOD_NOTES) return 1;

	for(new j = pos + 1, time; j < MAX_GOOD_NOTES; j++)
	{
		if(GoodNoteData[playerid][j][GOOD_NOTE_ID] == 0) continue;
		if(GoodNoteData[playerid][j][GOOD_NOTE_TIME_OUT] == 0) {
			CreateGoodNote(
				playerid, GoodNoteData[playerid][j][GOOD_NOTE_TEXT], 0, 
				GoodNoteData[playerid][j][GOOD_NOTE_COL_TEXT], GoodNoteData[playerid][j][GOOD_NOTE_COL_BG], 0
			);
		} else {
			time = GoodNoteData[playerid][j][GOOD_NOTE_TIME_OUT] - gettime();
			if(time > 0) {
				CreateGoodNote(
					playerid, GoodNoteData[playerid][j][GOOD_NOTE_TEXT], time, 
					GoodNoteData[playerid][j][GOOD_NOTE_COL_TEXT], GoodNoteData[playerid][j][GOOD_NOTE_COL_BG], 0
				);
			}
		}
		DestroyGoodNote(playerid, GoodNoteData[playerid][j][GOOD_NOTE_ID]);
		return 1;
	}
	return 1;
}

static stock gn_FindGoodNotePosById(playerid, goodnoteid)
{
	for(new i = 0; i < MAX_GOOD_NOTES; i++)
	{
		if(GoodNoteData[playerid][i][GOOD_NOTE_ID] == goodnoteid) return i;
	}
	return -1;
}

static stock gn_GetFreeNotePos(playerid)
{
	for(new i = 0; i < MAX_GOOD_NOTES; i++)
	{
		if(GoodNoteData[playerid][i][GOOD_NOTE_ID] == 0) {
			return i;
		}
	}
	return -1;
}

static stock gn_GetLineOfString(const string[], const size = sizeof(string))
{
	new
		lines_count = 1,
		perv_step_pos = 0,
		step_pos = 0
	;
	for(new i = 0; i < size; i++)
	{
		if(string[i] == '\0') break;
		step_pos = strfind(string, "~n~", false, perv_step_pos);
		if(step_pos == -1) {
			break;
		} else {
			perv_step_pos = step_pos + 3;
			lines_count++;
		}
	}
	return lines_count;
}
