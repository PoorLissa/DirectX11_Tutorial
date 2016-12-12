#pragma once
#ifndef _GLOBAL_DEFINES_H_
#define _GLOBAL_DEFINES_H_

// Let us put our global defines here
// ------------------------------------------------------------------------------------------------------------------------

#define MONSTERS_QTY             1

#define fullScreen
#undef  fullScreen

#define appTimerInterval         20             // Главный таймер приложения генерит событие onTimer по прошествии этого интервала в миллисекундах
#define EFFECT_DEFAULT_LENGTH    600            // Длительность (в тактах приложения) бонусных эффектов
#define SLOW_EFFECT_FACTOR       3              // Коэффициент замедления для бонуса SLOW

#define PLAYER_DEFAULT_HEALTH    100            // Начальное здоровье Игрока
#define MONSTER_DEFAULT_HEALTH   100            // Дефолтное здоровье монстров
#define BULLET_DEFAULT_HEALTH    100            // Дефолтное здоровье пули

#define BONUS_DROP_CHANCE        1000           // Величина, обратная шансу выпадения бонуса из монстра
#define WEAPON_DROP_CHANCE       10000          // Величина, обратная шансу выпадения оружия из монстра

#define useThread                               // Нужно ли использовать потоки (закомментить #undef, чтобы не исользовать потоки)
//#undef  useThread

#define singleShot                              // don't use single shot bullets
//#undef  singleShot

#endif
