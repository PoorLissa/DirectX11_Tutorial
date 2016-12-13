#pragma once
#ifndef _GLOBAL_DEFINES_H_
#define _GLOBAL_DEFINES_H_

// Let us put our global defines here
// ------------------------------------------------------------------------------------------------------------------------

#define MONSTERS_QTY             1000

#define fullScreen
#undef  fullScreen

#define appTimerInterval         20             // ������� ������ ���������� ������� ������� onTimer �� ���������� ����� ��������� � �������������
#define EFFECT_DEFAULT_LENGTH    600            // ������������ (� ������ ����������) �������� ��������
#define SLOW_EFFECT_FACTOR       3              // ����������� ���������� ��� ������ SLOW

#define PLAYER_DEFAULT_HEALTH    100            // ��������� �������� ������
#define MONSTER_DEFAULT_HEALTH   100            // ��������� �������� ��������
#define BULLET_DEFAULT_HEALTH    100            // ��������� �������� ����

#define BONUS_DROP_CHANCE        1000           // ��������, �������� ����� ��������� ������ �� �������
#define WEAPON_DROP_CHANCE       10000          // ��������, �������� ����� ��������� ������ �� �������

#define useThread                               // ����� �� ������������ ������
//#undef  useThread

#define useSorting                              // ���� �� �����, ���������� ������ ���������, ��� ��������� �������
#undef  useSorting

#endif
