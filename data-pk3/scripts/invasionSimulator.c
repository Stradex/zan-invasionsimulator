#library "invasionSimulator"
#include "zcommon.acs"

#include "defs.acs"
#include "funcs.acs"
#include "compabilityFuncs.acs"
#include "dinamicMonsters.acs"
//Dinamic Missions BASE
#include "itemsTracker.acs"
#include "iwadsBase.acs"
#include "dinamicMissions.acs"

//#include "test.acs" //para testear cosas, mas que nada matematicas

int started=0;
bool giveMegaToPlayers = true;
script 8055 OPEN
{
	int i, j;
	invasionStarted = false;
	int rand = GetCVar("cv_gmtime") % 256;
    for (i = 0; i < rand; i++) Random(0,255);

	//ACS_ExecuteAlways(TESTMATH, 0);
	delay(10); //Por si las moscas le damos un respiro al programa de 1 segundo
	itemRespawnEnabled = GetCvar("SV_ItemRespawn");
	//Limpio algunos mensajes
	SetCvar("alwaysapplydmflags", 1);
	SetCvar("sv_noexit", 1); //No permito irse del mapa a nadie
	ConsoleCommand("alwaysapplydmflags true");
	ConsoleCommand("sv_noexit true");
	HudMessageBold(s:""; HUDMSG_PLAIN, DMMSGS_MISSION, CR_RED, 1.5, 0.15, 1.0);
	HudMessageBold(s:""; HUDMSG_PLAIN, DMMSGS_STATE, CR_GOLD, 1.9, 0.75, 1.0);
	HudMessageBold(s:""; HUDMSG_PLAIN, DMMSGS_WAVESTATE, CR_GOLD, 1.9, 0.75, 1.0);
	HudMessageBold(s:""; HUDMSG_PLAIN, DMMSGS_MISSION, CR_RED, 1.5, 0.15, 1.0);
	HudMessageBold(s:""; HUDMSG_PLAIN, DMMSGS_PROGRESS, CR_GOLD, 1.9, 0.74, 1.0);

	gameIsSurvival = GetCvar("Survival");
	giveMegaToPlayers = GetCvar("is_givemega");
	for (i=0; i < MAXPLAYERS; i++)
	{
		Palive[i] = FALSE;
		PactPos[i][0] = -1;
		PactPos[i][1] = -1;
		PactPos[i][2] = -1;
		getSoulsphere[i] = false;
		PlayerAngle[i] = 0;
		RecentRespawned[i] = false;
		cActOldPos[i] = 0;
		cRecentPos[i] = 0;
		showingWeaponAcquiredHUD[i] = false;
		playerLives[i] = -1;
		for (j=0; j < MAXOLDPOS; j++)
		{
			PactOldPos[i][j][0] = -1;
			PactOldPos[i][j][1] = -1;
			PactOldPos[i][j][2] = -1;
		}
		for (j=0; j < MAXOLDPOS; j++)
		{
			PrecentPos[i][j][0] = -1;
			PrecentPos[i][j][1] = -1;
			PrecentPos[i][j][2] = -1;
		}
	}

	while (PlayerCount() <= 0)
		delay(20);	
		
	started = 1;

	delay(15); //Otro respiro mas
	dmTotalMonsters = GetLevelInfo(LEVELINFO_TOTAL_MONSTERS);
	dmdificultad = GetCvar("is_skill");
	delay(1);
	if (currentIwad == IWAD_STRIFE)
		strifeBlockExit(getCurrentMapName());
	//ACS_ExecuteAlways(SERVER_ITEMSTRACKER, 0); WTF?, En que estaba pensando jajajaj
	delay(35);
	ACS_ExecuteAlways(DMISSIONS_START, 0);
	
	bool skipMapOnLose = GetCvar("is_skipwhenlose");
	if ((currentIwad == IWAD_STRIFE) || !skipMapOnLose) //Para no pisar el survival de strafe
		terminate;
	do
	{
		delay(2);
	}while(checkPlayersAlive() || !invasionStarted);
	HudMessageBold(s:"This is what you want, this is what you get."; HUDMSG_PLAIN, DMMSGS_MISSION, CR_RED, 1.5, 0.15, 6.0);
	delay(35);
	Exit_Normal(0);
}

script 6066 ENTER
{
	do
	{
		delay(5);
	}while (!started);
	
	currentIwad = CheckIWAD(); //Verificar el iwad :D
	//log(s: "Iwad: ", d:currentIwad);

	//Se ejecuta el script comenzada la ronda
	Palive[PlayerNumber()] = TRUE;
	playerLives[PlayerNumber()] = GetPlayerLivesLeft(PlayerNumber())+1;
	//Obtener posicion inicial del jugador, para modo Resurreccion
	int x = GetActorX(0);
	int y = GetActorY(0);
	int z = GetActorZ(0);
	//ACS_ExecuteAlways(670, 0, PlayerNumber());
	int tPassed = 980;
	int rPassed = 9; //casi 1/4 de segundo
	int rInventory = 70;
	int tmpLife = GetActorProperty(0, APROP_HEALTH);
	bool backpackinv = false;
	int invTime = 175; //5 segundos de inmortalidad
	str tmpAClass = GetActorClass(0);
		ACS_ExecuteAlways(SERVER_GIVE_KEYS , 0);
	while(PlayerInGame(PlayerNumber())) {
		if (!Palive[PlayerNumber()])
		{
			delay(17);
			continue;
		}
		tmpAClass = GetActorClass(0);
		PactPos[PlayerNumber()][0] = GetActorX(0);
		PactPos[PlayerNumber()][1] = GetActorY(0);
		PactPos[PlayerNumber()][2] = GetActorZ(0);
		PlayerAngle[PlayerNumber()] = GetActorAngle(0);
		//HudMessageBold(s:"X: ", f:PactPos[PlayerNumber()][0], s:" - Y: ", f:PactPos[PlayerNumber()][1], s:" - Z: ", f:PactPos[PlayerNumber()][1]; HUDMSG_PLAIN, 5, CR_GOLD, 1.8, 0.8, 1);
		if (RecentRespawned[PlayerNumber()])
		{
			invTime--;
			if (invTime <= 0)
			{
				invTime = 175;
				//SetActorProperty(0, APROP_Invulnerable, FALSE); //Le sacamo la inmortalidad al pibe
				RecentRespawned[PlayerNumber()] = false;
				//SetPlayerProperty(FALSE, OFF, PROP_BUDDHA);
				fadeTo(115, 162, 255, 0.0, 0.5);
			} else {
				tmpLife = GetActorProperty(0, APROP_HEALTH);
				if (tmpLife < 200)
					SetActorProperty(0, APROP_HEALTH, 200);
			}
		}
		tPassed--;
		if (tPassed <=0)
		{
			PactOldPos[PlayerNumber()][cActOldPos[PlayerNumber()]][0] = GetActorX(0);
			PactOldPos[PlayerNumber()][cActOldPos[PlayerNumber()]][1] = GetActorY(0);
			PactOldPos[PlayerNumber()][cActOldPos[PlayerNumber()]][2] = GetActorZ(0);

			cActOldPos[PlayerNumber()]++;
			if (cActOldPos[PlayerNumber()] >= MAXOLDPOS)
				cActOldPos[PlayerNumber()] = 0;
			tPassed = 980;
		}
		if (DMCurrentMission == DMISSIONS_INVASION)
		{
				rInventory --;
				if (dminvasionCountdown)
				{
				if (!backpackinv)
				{
					GiveInventory("Backpack", 1);
					if (giveMegaToPlayers)
						GiveInventory("Megasphere", 1);
					delay(1);
					backpackinv = true;
				}
				if (getSoulsphere[PlayerNumber()] && giveMegaToPlayers)
				{
					GiveInventory("Megasphere", 1);
					/*tmpLife = GetActorProperty(acTid, APROP_HEALTH);
					tmpLife = tmpLife + 100;
					if (tmpLife > 200)
						tmpLife = 200;
					SetActorProperty(acTid, APROP_HEALTH, tmpLife);*/
					//Fue, ahora es re jodido, hace falta la mega siempre
					getSoulsphere[PlayerNumber()] = false;
				}
				if (!CheckInventory(convertirArma(tmpAClass, "Shotgun")))
				{
					if (!showingWeaponAcquiredHUD[PlayerNumber()])
					{
						ACS_ExecuteAlways(DMINVASION_HUDWEAPONCS, 0, DMLanguage);
						showingWeaponAcquiredHUD[PlayerNumber()] = true;
					}
					GiveInventory(convertirArma(tmpAClass, "Shotgun"), 1);
				}	
				if (!CheckInventory(convertirArma(tmpAClass, "Pistol")))
					GiveInventory(convertirArma(tmpAClass, "Pistol"), 1);
					
				if (rInventory  <= 0)
				{
				
				if (CheckInventory(convertirArma(tmpAClass, "Pistol")) && CheckInventory(convertirMunicion(tmpAClass, 0)) < 300)
					GiveInventory(convertirMunicion(tmpAClass, 0), 12);
					
				if (CheckInventory(convertirArma(tmpAClass,"Shotgun")) && CheckInventory(convertirMunicion(tmpAClass, 1)) < 75)
					GiveInventory(convertirMunicion(tmpAClass, 1), 6);
					
				if (CheckInventory(convertirArma(tmpAClass,"SuperShotgun")) && CheckInventory(convertirMunicion(tmpAClass, 2)) < 75)
					GiveInventory(convertirMunicion(tmpAClass, 2), 6);
					
				if (CheckInventory(convertirArma(tmpAClass,"Chaingun")) && CheckInventory(convertirMunicion(tmpAClass, 3)) < 300)
					GiveInventory(convertirMunicion(tmpAClass, 3), 12);
					
				if (CheckInventory(convertirArma(tmpAClass,"RocketLauncher")) && CheckInventory(convertirMunicion(tmpAClass, 4)) < 50)
					GiveInventory(convertirMunicion(tmpAClass, 4), 3);
					
				if (CheckInventory(convertirArma(tmpAClass,"PlasmaRifle")) && CheckInventory(convertirMunicion(tmpAClass, 5)) < 300)
					GiveInventory(convertirMunicion(tmpAClass, 5), 3);	
					
				if ((dminvasionFase == 1) && !(CheckInventory(convertirArma(tmpAClass, "Chaingun"))))
				{
					if (!showingWeaponAcquiredHUD[PlayerNumber()])
					{
						ACS_ExecuteAlways(DMINVASION_HUDWEAPONCS, 0, DMLanguage);
						showingWeaponAcquiredHUD[PlayerNumber()] = true;
					}
					GiveInventory(convertirArma(tmpAClass, "Chaingun"), 1);
					
				}
				if ((dminvasionFase == 2) && !(CheckInventory(convertirArma(tmpAClass, "SuperShotgun"))))
				{
					GiveInventory(convertirArma(tmpAClass, "SuperShotgun"), 1);
					if (!showingWeaponAcquiredHUD[PlayerNumber()])
					{
						ACS_ExecuteAlways(DMINVASION_HUDWEAPONCS, 0, DMLanguage);
						showingWeaponAcquiredHUD[PlayerNumber()] = true;
					}
				}
				if (dminvasionFase == 3)
				{
					if (!(CheckInventory(convertirArma(tmpAClass, "RocketLauncher"))))
						GiveInventory(convertirArma(tmpAClass, "RocketLauncher"), 1);
						
					if (!showingWeaponAcquiredHUD[PlayerNumber()])
					{
						ACS_ExecuteAlways(DMINVASION_HUDWEAPONCS, 0, DMLanguage);
						showingWeaponAcquiredHUD[PlayerNumber()] = true;
					}
					/*if ((currentIwad == 3) && !CheckInventory("Sigil"))  //Strife
						GiveInventory("Sigil", 1); //Aniquilador de masas 500*/
				}
				if (dminvasionFase == 4)
				{
					/*if ((currentIwad == 3) && !CheckInventory("Sigil5"))   //Strife
						GiveInventory("Sigil5", 1); //Aniquilador de masas 5000*/
						
					if (!showingWeaponAcquiredHUD[PlayerNumber()])
					{
						ACS_ExecuteAlways(DMINVASION_HUDWEAPONCS, 0, DMLanguage);
						showingWeaponAcquiredHUD[PlayerNumber()] = true;
					}
					if (!(CheckInventory(convertirArma(tmpAClass, "PlasmaRifle"))))
						GiveInventory(convertirArma(tmpAClass, "PlasmaRifle"), 1);
				}
				//if ((dminvasionFase == 5) && !(CheckInventory("BFG9000")))
				//	GiveInventory("BFG9000", 1);
				rInventory  = 105;
				
				}
				} else {
					if (rInventory  <= 0)
					{
						if (dminvasionFase >= 0)
						{
							if (CheckInventory(convertirArma(tmpAClass, "Pistol")) && CheckInventory(convertirMunicion(tmpAClass, 0)) < 300)
								GiveInventory(convertirMunicion(tmpAClass, 0), 2);
							if ((currentIwad ==  IWAD_STRIFE) && CheckInventory(convertirArma(tmpAClass,"Chaingun")) && CheckInventory(convertirMunicion(tmpAClass, 3)) < 300)
								GiveInventory(convertirMunicion(tmpAClass, 3), 2);
						}
						if (dminvasionFase >= 1)
						{
							if (CheckInventory(convertirArma(tmpAClass,"Shotgun")) && CheckInventory(convertirMunicion(tmpAClass, 1)) < 75)
								GiveInventory(convertirMunicion(tmpAClass, 1), 1);
						}
						if (dminvasionFase >= 2)
						{
							if (CheckInventory(convertirArma(tmpAClass,"Shotgun")) && CheckInventory(convertirMunicion(tmpAClass, 1)) < 75)
								GiveInventory(convertirMunicion(tmpAClass, 1), 1);
							if (CheckInventory(convertirArma(tmpAClass,"SuperShotgun")) && CheckInventory(convertirMunicion(tmpAClass, 2)) < 75)
								GiveInventory(convertirMunicion(tmpAClass, 2), 1);
						}
						if (dminvasionFase >= 4)
						{
							if (CheckInventory(convertirArma(tmpAClass,"RocketLauncher")) && CheckInventory(convertirMunicion(tmpAClass, 4)) < 50)
								GiveInventory(convertirMunicion(tmpAClass, 4), 3);
						}
						rInventory  = 210;
					}
				}
		}
		rPassed--;
		if (rPassed <= 0)
		{
			PrecentPos[PlayerNumber()][cRecentPos[PlayerNumber()]][0] = GetActorX(0);
			PrecentPos[PlayerNumber()][cRecentPos[PlayerNumber()]][1] = GetActorY(0);
			PrecentPos[PlayerNumber()][cRecentPos[PlayerNumber()]][2] = GetActorZ(0);

			cRecentPos[PlayerNumber()]++;
			if (cRecentPos[PlayerNumber()] >= MAXOLDPOS)
				cRecentPos[PlayerNumber()] = 0;
			
			rPassed = 9;
		}
		delay(1);
	}
}

script 6067 DEATH
{
	delay(1);
	Palive[PlayerNumber()] = FALSE;
	playerLives[PlayerNumber()] = GetPlayerLivesLeft(PlayerNumber());
	//Log(s: "Vidas: ", d:playerLives[PlayerNumber()], s:" | Invasion: ", d:invasionStarted);
}

script 6068 RESPAWN
{
	//SetActorProperty(0, APROP_Invulnerable, TRUE);
	//SetPlayerProperty(FALSE, ON, PROP_BUDDHA);
	delay(1);

	Palive[PlayerNumber()] = TRUE;
	RecentRespawned[PlayerNumber()] = TRUE;
	delay(1);
	int acTid = MINPLAYERTID + PlayerNumber();
	int tmpLife = GetActorProperty(0, APROP_HEALTH);
	tmpLife = tmpLife + 100;
	if (tmpLife > 200)
		tmpLife = 200;
	SetActorProperty(0, APROP_HEALTH, tmpLife);
	fadeTo(115, 162, 255, 0.3, 0.5);
}

script 6069 (int gone) DISCONNECT 
{
	Palive[gone] = FALSE;
}