#pragma once
#ifndef _GLOBAL_DEFINES_H_
#define _GLOBAL_DEFINES_H_

// Let us put our global defines here, which is actually a poor choice since cnahging this global defines rebuilds all the project
// ------------------------------------------------------------------------------------------------------------------------

#define MONSTERS_QTY             1

#define fullScreen
#undef  fullScreen

#define appTimerInterval         20             // Интервал в миллисекундах, по истечении которого главный таймер приложения генерит событие onTimer
#define EFFECT_DEFAULT_LENGTH    600            // Длительность бонусных эффектов (в тактах приложения)
#define SLOW_EFFECT_FACTOR       3              // Коэффициент замедления для бонуса SLOW
#define BONUS_DEFAULT_LIFESPAN   5000           // Время жизни бонуса на карте

#define PLAYER_DEFAULT_HEALTH    100            // Начальное здоровье Игрока
#define MONSTER_DEFAULT_HEALTH   100            // Дефолтное здоровье монстров
#define BULLET_DEFAULT_HEALTH    100            // Дефолтное здоровье пули

#define BONUS_DROP_CHANCE        1000           // Величина, обратная шансу выпадения бонуса из монстра
#define WEAPON_DROP_CHANCE       10000          // Величина, обратная шансу выпадения оружия из монстра

#define useThread                               // Нужно ли использовать потоки
//#undef  useThread

#define BULLET_BONUS_LIFE        0              // для гауссового оружия



// ------------------------------------------------------------------------------------------------------------------------



inline void logMsg(const std::string &str, bool doCleanFile = false, char *fileName = "___msgLog.log")
{
	FILE *f = NULL;

	fopen_s(&f, fileName, doCleanFile ? "w" : "a");
	if( f != NULL ) {
		fputs(str.c_str(), f);
		fputs("\n", f);
		fclose(f);
	}
}
// ------------------------------------------------------------------------------------------------------------------------

#endif
